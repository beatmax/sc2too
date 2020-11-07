#include "tests-rts-seq-run.h"

#include "rts/World.h"
#include "rts/constants.h"
#include "tests-rts-assets.h"
#include "tests-rts-util.h"
#include "util/algorithm.h"

#include <functional>
#include <map>

namespace {
  enum class Property { Unknown, Quantity };

  Property toProperty(const std::string& s) {
    if (s == "quantity")
      return Property::Quantity;
    return Property::Unknown;
  }

  rts::AbilityInstanceIndex toAbilityIndex(const std::string& s) {
    if (s == "move")
      return test::MoveAbilityIndex;
    if (s == "gather")
      return test::GatherAbilityIndex;
    if (s == "build")
      return test::BuildAbilityIndex;
    if (s == "produce_worker")
      return test::ProduceWorkerAbilityIndex;
    if (s == "produce_thirdy")
      return test::ProduceThirdyAbilityIndex;
    if (s == "set_rally_point")
      return test::SetRallyPointAbilityIndex;
    return rts::AbilityInstanceIndex::None;
  }

  bool requiresTarget(const std::string& s) {
    return s != "produce_worker" && s != "produce_thirdy";
  }

  rts::UnitTypeId toUnitTypeId(const std::string& s) {
    if (s == "building")
      return test::buildingTypeId;
    if (s == "worker")
      return test::workerTypeId;
    if (s == "thirdy")
      return test::thirdyTypeId;
    return {};
  }

  std::string toString(rts::UnitTypeId t) {
    if (t == test::buildingTypeId)
      return "building";
    if (t == test::workerTypeId)
      return "worker";
    if (t == test::thirdyTypeId)
      return "thirdy";
    return "?";
  }

  rts::ResourceId toResourceId(const std::string& s) {
    if (s == "gas")
      return test::gasResourceId;
    if (s == "supply")
      return test::supplyResourceId;
    return {};
  }

  bool isIdle(const rts::Unit& u) {
    return rts::Unit::nextStepTime(rts::UnitStableRef{u}) == rts::GameTimeInf;
  }

  bool stepWorld(rts::World& w, rts::GameTime limit) {
    using namespace rts;

    const auto it{util::minElementBy(
        w.units, [](const auto& u) { return Unit::nextStepTime(UnitStableRef{u}); })};
    if (it == w.units.end())
      return false;

    const auto nextTime{Unit::nextStepTime(UnitStableRef{*it})};
    if (nextTime > limit) {
      if (nextTime != GameTimeInf)
        w.time = limit;
      return false;
    }

    w.time = nextTime;
    for (auto& u : w.units)
      w.update(Unit::step(StableRef{u}, w));

    return true;
  }
}

namespace test::seq {
  namespace {
    struct Options {
      bool mapTime{false};
      bool mapGas{false};
      bool mapSupply{false};
    };

    struct Runner {
      rts::World& world;
      rts::SideId side;
      const Sequence& input;
      Sequence& output;
      Options options;

      std::map<rts::UnitId, char> unitName;
      std::map<char, rts::UnitId> unitByName;
      bool haveMap{false};
      rts::UnitId prototype{};

      void operator()();

    private:
      void operator()(const item::Error& e);
      void operator()(const item::Empty& e);
      void operator()(const item::Comment& c);
      void operator()(const item::Option& o);
      void operator()(const item::Definition& d);
      void operator()(const item::Assignment& a);
      void operator()(const item::Map& m);
      void operator()(const item::Message&) {}
      void operator()(const item::Prototype&) {}
      void operator()(const item::Run& r);
      void operator()(const item::Action& a);
      void operator()(const item::Command& c);

      void error(const Item& i, const std::string& s);
      item::Map makeMap();
      void action(const item::act::Add& a);
      void action(const item::act::Destroy& d);
      void action(const item::act::Provision& p);
      void action(const item::act::Allocate& a);
      void command(const item::cmd::BuildPrototype& bp);
      void command(const item::cmd::Select& s);
      void command(const item::cmd::Trigger& t);
      void addEvents();
      void addMessages();
      void addPrototype();
    };

    void Runner::operator()() {
      for (const Item& item : input)
        std::visit([this](const auto& i) { (*this)(i); }, item);
    }

    void Runner::operator()(const item::Error& e) { output.push_back(e); }
    void Runner::operator()(const item::Empty& e) { output.push_back(e); }
    void Runner::operator()(const item::Comment& c) { output.push_back(c); }

    void Runner::operator()(const item::Option& o) {
      for (auto& opt : o.options) {
        if (opt == "t")
          options.mapTime = true;
        else if (opt == "g")
          options.mapGas = true;
        else if (opt == "s")
          options.mapSupply = true;
        else
          return error(o, "invalid option: " + opt);
      }
      output.push_back(o);
    }

    void Runner::operator()(const item::Definition& d) {
      auto type{toUnitTypeId(d.type)};
      if (!type)
        return error(d, "invalid unit type");
      for (const auto& name : d.names) {
        if (name.size() != 1)
          return error(d, "name size must be 1: " + name);
        char n{name.front()};
        if (unitByName.find(n) != unitByName.end())
          return error(d, "duplicate definition: " + n);
        auto u{world.createUnit(type, side)};
        unitName[u] = n;
        unitByName[n] = u;
      }
      output.push_back(d);
    }

    void Runner::operator()(const item::Assignment& a) {
      const rts::Cell& cell{world[a.target]};
      auto property{toProperty(a.property)};
      switch (property) {
        case Property::Unknown:
          return error(a, "invalid property name");
        case Property::Quantity:
          if (!cell.contains(rts::Cell::ResourceField))
            return error(a, "target is not a resource field");
          world[cell.resourceFieldId()].bag.setQuantity(a.value);
          break;
      }
      output.push_back(a);
    }

    void Runner::operator()(const item::Map& m) {
      if (haveMap)
        return;

      world.loadMap(test::MapInitializer{unitByName}, m.map);
      haveMap = true;
      output.push_back(makeMap());
    }

    void Runner::operator()(const item::Run& r) {
      if (!haveMap)
        return error(r, "map not initialized");

      std::function<bool()> stop;
      if (r.untilIdle) {
        if (r.untilIdle->size() != 1)
          return error(r, "name size must be 1");
        auto it{unitByName.find(r.untilIdle->front())};
        if (it == unitByName.end())
          return error(r, "undefined name: " + *r.untilIdle);
        stop = [this, u{it->second}]() { return isIdle(world[u]); };
      }
      else {
        stop = []() { return false; };
      }

      auto map{makeMap()};
      output.push_back(r);
      output.push_back(map);

      const auto limit{world.time + (r.duration ? *r.duration : rts::GameTimeSecond * 1000)};

      while (!stop() && stepWorld(world, limit)) {
        auto newMap{makeMap()};
        if (newMap.diff(map)) {
          map = std::move(newMap);
          output.push_back(map);
        }
        addEvents();
      }

      if (world.time < limit && r.duration)
        world.time = limit;

      if (world.time != map.time)
        output.push_back(makeMap());

      if (world.time >= limit && !r.duration)
        output.push_back(item::Error{"!!! time limit reached"});
    }

    void Runner::operator()(const item::Action& a) {
      if (!haveMap)
        return error(a, "map not initialized");
      std::visit([this](const auto& a) { action(a); }, a);
      addEvents();
    }

    void Runner::operator()(const item::Command& c) {
      if (!haveMap)
        return error(c, "map not initialized");
      std::visit([this](const auto& c) { command(c); }, c);
      addEvents();
    }

    void Runner::error(const Item& i, const std::string& s) {
      output.push_back(item::Error{toString(i) + " <--- " + s});
    }

    item::Map Runner::makeMap() {
      item::Map m;
      const auto& size{world.map.size()};
      for (rts::Coordinate y{0}; y < size.y; ++y)
        m.map.push_back(std::string(size.x, ' '));
      for (auto p : world.map.area().points()) {
        const rts::Cell& cell{world[p]};
        if (const auto* obj{world.object(cell)}) {
          auto it{unitName.find(cell.unitId())};
          m.map[p.y][p.x] = (it != unitName.end()) ? it->second : test::repr(obj->ui).front();
        }
      }
      if (options.mapTime)
        m.time = world.time;
      if (options.mapGas)
        m.gas = world[side].resource(test::gasResourceId).available();
      if (options.mapSupply) {
        const auto& supply = world[side].resource(test::supplyResourceId);
        m.supply = {supply.allocated(), supply.totalSlots()};
      }
      return m;
    }

    void Runner::action(const item::act::Add& a) {
      if (a.name.size() != 1)
        return error(a, "name size must be 1: " + a.name);
      if (auto it{unitByName.find(a.name.front())}; it == unitByName.end())
        return error(a, "undefined name: " + a.name);
      else
        world.addForFree(it->second, a.point);
      output.push_back(a);
    }

    void Runner::action(const item::act::Destroy& d) {
      for (const auto& name : d.names) {
        if (name.size() != 1)
          return error(d, "name size must be 1: " + name);
        if (auto it{unitByName.find(name.front())}; it == unitByName.end())
          return error(d, "undefined name: " + name);
        else {
          auto u{it->second};
          unitByName.erase(it);
          unitName.erase(u);
          world.destroy(u);
        }
      }
      output.push_back(d);
    }

    void Runner::action(const item::act::Provision& p) {
      auto resource{toResourceId(p.resource)};
      if (!resource)
        return error(p, "invalid resource name");
      world[side].resources().provision({{resource, p.quantity}});
      output.push_back(p);
    }

    void Runner::action(const item::act::Allocate& p) {
      auto resource{toResourceId(p.resource)};
      if (!resource)
        return error(p, "invalid resource name");
      world[side].resources().allocate({{resource, p.quantity}});
      output.push_back(p);
    }

    void Runner::command(const item::cmd::BuildPrototype& bp) {
      auto type{toUnitTypeId(bp.type)};
      if (!type)
        return error(bp, "invalid unit type");
      test::execCommand(world, side, rts::command::BuildPrototype{type});
      output.push_back(bp);
    }

    void Runner::command(const item::cmd::Select& s) {
      rts::UnitIdList units;
      units.reserve(s.names.size());
      for (const auto& name : s.names) {
        if (name.size() != 1)
          return error(s, "name size must be 1: " + name);
        if (auto it{unitByName.find(name.front())}; it == unitByName.end())
          return error(s, "undefined name: " + name);
        else
          units.push_back(it->second);
      }
      test::select(world, side, units);
      output.push_back(s);
    }

    void Runner::command(const item::cmd::Trigger& t) {
      auto ai{toAbilityIndex(t.ability)};
      if (ai == rts::AbilityInstanceIndex::None)
        return error(t, "invalid ability");
      if (!t.target && requiresTarget(t.ability))
        return error(t, "target required");
      test::execCommand(
          world, side, rts::command::TriggerAbility{ai, t.target ? *t.target : rts::Point{-1, -1}});
      output.push_back(t);
    }

    void Runner::addEvents() {
      addMessages();
      addPrototype();
    }

    void Runner::addMessages() {
      auto& messages{world[side].messages()};
      for (size_t i{0}; i < messages.size(); ++i)
        output.push_back(item::Message{messages[i].text});
      messages.clear();
    }

    void Runner::addPrototype() {
      if (auto p{world[side].prototype()}) {
        output.push_back(item::Prototype{::toString(world[p].type)});
        prototype = p;
      }
      else if (prototype) {
        output.push_back(item::Prototype{"none"});
        prototype = {};
      }
    }
  }
}

auto test::seq::run(const Sequence& input) -> Sequence {
  Sequence output;

  auto worldPtr{rts::World::create(std::make_unique<test::Factory>())};
  rts::World& world{*worldPtr};
  test::makeSides(world);

  Runner{world, test::side1Id, input, output}();
  return output;
}
