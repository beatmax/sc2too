#include "tests-rts-seq-run.h"

#include "rts/World.h"
#include "rts/constants.h"
#include "tests-rts-assets.h"
#include "tests-rts-util.h"
#include "util/algorithm.h"

#include <functional>
#include <map>

namespace {
  enum class Property { Unknown, Quantity, Abilities, WorkerCount };

  Property toProperty(const std::string& s) {
    if (s == "quantity")
      return Property::Quantity;
    if (s == "abilities")
      return Property::Abilities;
    if (s == "worker_count")
      return Property::WorkerCount;
    return Property::Unknown;
  }

  rts::AbilityInstanceIndex toAbilityIndex(const std::string& s) {
    if (s == "boost")
      return test::BoostAbilityIndex;
    if (s == "build_base")
      return test::BuildBaseAbilityIndex;
    if (s == "build_dojo")
      return test::BuildDojoAbilityIndex;
    if (s == "build_extractor")
      return test::BuildExtractorAbilityIndex;
    if (s == "build_lab")
      return test::BuildLabAbilityIndex;
    if (s == "build_power_plant")
      return test::BuildPowerPlantAbilityIndex;
    if (s == "gather")
      return test::GatherAbilityIndex;
    if (s == "move")
      return test::MoveAbilityIndex;
    if (s == "produce_fighter")
      return test::ProduceFighterAbilityIndex;
    if (s == "produce_soldier")
      return test::ProduceSoldierAbilityIndex;
    if (s == "produce_worker")
      return test::ProduceWorkerAbilityIndex;
    if (s == "produce_thirdy")
      return test::ProduceThirdyAbilityIndex;
    if (s == "research_level1")
      return test::ResearchLevel1AbilityIndex;
    if (s == "set_rally_point")
      return test::SetRallyPointAbilityIndex;
    return rts::AbilityInstanceIndex::None;
  }

  bool requiresTarget(const std::string& s) {
    return s.rfind("produce_", 0) != 0 && s.rfind("research_", 0) != 0;
  }

  rts::UnitTypeId toUnitTypeId(const std::string& s) {
    if (s == "base")
      return test::baseTypeId;
    if (s == "dojo")
      return test::dojoTypeId;
    if (s == "extractor")
      return test::extractorTypeId;
    if (s == "fighter")
      return test::fighterTypeId;
    if (s == "lab")
      return test::labTypeId;
    if (s == "power_plant")
      return test::powerPlantTypeId;
    if (s == "soldier")
      return test::soldierTypeId;
    if (s == "thirdy")
      return test::thirdyTypeId;
    if (s == "worker")
      return test::workerTypeId;
    return {};
  }

  std::string toString(rts::UnitTypeId t) {
    if (t == test::baseTypeId)
      return "base";
    if (t == test::dojoTypeId)
      return "dojo";
    if (t == test::extractorTypeId)
      return "extractor";
    if (t == test::fighterTypeId)
      return "fighter";
    if (t == test::labTypeId)
      return "lab";
    if (t == test::powerPlantTypeId)
      return "power_plant";
    if (t == test::soldierTypeId)
      return "soldier";
    if (t == test::thirdyTypeId)
      return "thirdy";
    if (t == test::workerTypeId)
      return "worker";
    return "?";
  }

  rts::ResourceId toResourceId(const std::string& s) {
    if (s == "gas")
      return test::gasResourceId;
    if (s == "mineral")
      return test::mineralResourceId;
    if (s == "supply")
      return test::supplyResourceId;
    return {};
  }

  bool isIdle(const rts::Unit& u) {
    return rts::Unit::nextStepTime(rts::UnitStableRef{u}) == rts::GameTimeInf &&
        u.commandQueue.empty();
  }

  void fastForward(rts::World& w, rts::GameTime t) {
    while (w.time < t)
      w.step();
  }

  bool stepWorld(rts::World& w, rts::GameTime limit) {
    using namespace rts;

    const auto it{util::minElementBy(
        w.units, [](const auto& u) { return Unit::nextStepTime(UnitStableRef{u}); })};
    if (it == w.units.end())
      return false;

    auto nextTime{Unit::nextStepTime(UnitStableRef{*it})};
    if (nextTime == GameTimeInf && !it->commandQueue.empty())
      nextTime = w.time + 1;

    if (nextTime > limit) {
      if (nextTime != GameTimeInf)
        fastForward(w, limit);
      return false;
    }

    fastForward(w, nextTime);
    return true;
  }
}

namespace test::seq {
  namespace {
    struct Options {
      bool mapTime{false};
      bool mapResources{false};
      bool mapSupply{false};
      bool mapUpgrades{false};
      bool mapEnergy{false};
    };

    struct Runner {
      rts::World& world;
      rts::SideId side;
      const Sequence& input;
      Sequence& output;
      Options options;

      std::map<rts::UnitId, std::string> unitName;
      std::map<std::string, rts::UnitId> unitByName;
      bool haveMap{false};
      rts::UnitId prototype{};

      void operator()();

    private:
      void operator()(const item::Error& e);
      void operator()(const item::Empty& e);
      void operator()(const item::Comment& c);
      void operator()(const item::Option& o);
      void operator()(const item::Definition& d);
      void operator()(const item::Reference& r);
      void operator()(const item::Assignment& a);
      void operator()(const item::Expectation& e);
      void operator()(const item::Map& m);
      void operator()(const item::Message&) {}
      void operator()(const item::Prototype&) {}
      void operator()(const item::Run& r);
      void operator()(const item::Action& a);
      void operator()(const item::Command& c);

      void error(const Item& i, const std::string& s);
      item::Map makeMap(bool extraInfo = false);
      void action(const item::act::Add& a);
      void action(const item::act::Destroy& d);
      void action(const item::act::Provision& p);
      void action(const item::act::Allocate& a);
      void command(const item::cmd::Prepare& p);
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
        else if (opt == "r")
          options.mapResources = true;
        else if (opt == "s")
          options.mapSupply = true;
        else if (opt == "u")
          options.mapUpgrades = true;
        else if (opt == "e")
          options.mapEnergy = true;
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
        if (unitByName.find(name) != unitByName.end())
          return error(d, "duplicate definition: " + name);
        auto u{world.createUnit(type, side)};
        unitName[u] = name;
        unitByName[name] = u;
      }
      output.push_back(d);
    }

    void Runner::operator()(const item::Reference& r) {
      auto u{world.unitId(r.point)};
      if (!u)
        return error(r, "no unit");
      if (unitByName.find(r.name) != unitByName.end())
        return error(r, "duplicate definition: " + r.name);
      unitName[u] = r.name;
      unitByName[r.name] = u;
      output.push_back(r);
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
          if (a.value >= 0)
            world[cell.resourceFieldId()].bag.setQuantity(a.value);
          else
            world.destroy(cell.resourceFieldId());
          break;
        case Property::Abilities:
        case Property::WorkerCount:
          return error(a, "cannot assign");
      }
      output.push_back(a);
    }

    void Runner::operator()(const item::Expectation& e) {
      rts::UnitId unit;
      if (auto it{unitByName.find(e.name)}; it == unitByName.end())
        return error(e, "undefined name: " + e.name);
      else
        unit = it->second;
      std::string value;
      auto property{toProperty(e.property)};
      switch (property) {
        case Property::Unknown:
          return error(e, "invalid property name");
        case Property::Quantity:
          return error(e, "cannot inspect");
        case Property::Abilities: {
          std::vector<std::string> abilities;
          const auto& u{world[unit]};
          const auto& t{world[u.type]};
          for (const auto& ai : t.abilities) {
            if (ai.kind != rts::abilities::Kind::None) {
              auto aId{ai.abilityId};
              if (auto rs{u.abilityReadyState(world, t.abilityIndex(aId), aId)};
                  rs != rts::AbilityReadyState::None) {
                abilities.push_back(
                    test::repr(world[aId].ui) +
                    (rs == rts::AbilityReadyState::Disabled ? "*" : ""));
              }
            }
          }
          value = '[' + util::join(abilities, ' ') + ']';
          break;
        }
        case Property::WorkerCount: {
          const auto& u{world[unit]};
          auto count = u.workerCount.value();
          auto saturation = u.resourceField ? world[u.resourceField].optimalWorkerCount
                                            : world.resourceBaseSaturationMap[u.area.center()];
          value = std::to_string(count) + '/' + std::to_string(saturation);
          break;
        }
      }
      output.push_back(item::Expectation{e.property, e.name, value});
    }

    void Runner::operator()(const item::Map& m) {
      if (haveMap)
        return;

      world.loadMap(test::MapInitializer{unitByName}, m.map);
      haveMap = true;
      output.push_back(makeMap(true));
    }

    void Runner::operator()(const item::Run& r) {
      if (!haveMap)
        return error(r, "map not initialized");

      std::function<bool()> stop;
      if (r.untilIdle) {
        auto it{unitByName.find(*r.untilIdle)};
        if (it == unitByName.end())
          return error(r, "undefined name: " + *r.untilIdle);
        stop = [this, u{it->second}]() { return isIdle(world[u]); };
      }
      else {
        stop = []() { return false; };
      }

      auto map{makeMap(true)};
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
        fastForward(world, limit);

      if (auto newMap{makeMap(true)}; newMap != map)
        output.push_back(makeMap(true));

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

    item::Map Runner::makeMap(bool extraInfo) {
      item::Map m;
      const auto& size{world.map.size()};
      for (rts::Coordinate y{0}; y < size.y; ++y)
        m.map.push_back(std::string(size.x, ' '));
      for (auto p : world.map.area().points()) {
        const rts::Cell& cell{world[p]};
        if (const auto* obj{world.object(cell)}) {
          auto it{unitName.find(cell.unitId())};
          m.map[p.y][p.x] =
              (it != unitName.end()) ? it->second.front() : test::repr(obj->ui).front();
        }
      }
      if (options.mapTime)
        m.time = world.time;
      if (options.mapResources) {
        m.gas = world[side].resource(test::gasResourceId).available();
        m.mineral = world[side].resource(test::mineralResourceId).available();
      }
      if (options.mapSupply) {
        const auto& supply = world[side].resource(test::supplyResourceId);
        m.supply = {supply.allocated(), supply.totalSlots()};
      }
      if (options.mapUpgrades) {
        const auto& s{world[side]};
        auto& upgrades{m.upgrades.emplace()};
        for (const auto& u : world.upgrades) {
          if (s.hasUpgrade(world.id(u)))
            upgrades.push_back(test::repr(u.ui));
        }
      }
      if (extraInfo && options.mapEnergy) {
        m.energy.emplace();
        for (const auto& [n, u] : unitByName) {
          const auto& unit{world[u]};
          if (world[unit.type].maxEnergy)
            m.energy->emplace_back(n, unit.energy);
        }
      }
      return m;
    }

    void Runner::action(const item::act::Add& a) {
      if (auto it{unitByName.find(a.name)}; it == unitByName.end())
        return error(a, "undefined name: " + a.name);
      else if (!world.map.isEmpty(rts::Rectangle{a.point, world[it->second].area.size}))
        return error(a, "area not empty");
      else
        world.addForFree(it->second, a.point);
      output.push_back(a);
    }

    void Runner::action(const item::act::Destroy& d) {
      for (const auto& name : d.names) {
        if (auto it{unitByName.find(name)}; it == unitByName.end())
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

    void Runner::command(const item::cmd::Prepare& p) {
      auto ai{toAbilityIndex(p.ability)};
      if (ai == rts::AbilityInstanceIndex::None)
        return error(p, "invalid ability");
      const auto& subgroup{world[side].selection().subgroup(world)};
      if (!subgroup.abilityReady(world, ai))
        return error(p, "ability not ready");
      test::execCommand(world, side, rts::command::PrepareAbility{ai});
      output.push_back(p);
    }

    void Runner::command(const item::cmd::Select& s) {
      rts::UnitIdList units;
      units.reserve(s.names.size());
      for (const auto& name : s.names) {
        if (auto it{unitByName.find(name)}; it == unitByName.end())
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
      const auto& subgroup{world[side].selection().subgroup(world)};
      if (!subgroup.abilityReady(world, ai))
        return error(t, "ability not ready");
      test::execCommand(
          world, side,
          rts::command::TriggerAbility{ai, t.target ? *t.target : rts::Point{-1, -1}, t.enqueue});
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
