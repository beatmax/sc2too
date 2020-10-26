#include "tests-rts-seq-run.h"

#include "rts/Engine.h"
#include "rts/World.h"
#include "rts/constants.h"
#include "tests-rts-assets.h"
#include "tests-rts-util.h"

#include <map>

namespace {
  using util::FakeClock;
  using Engine = rts::EngineBase<FakeClock>;

  enum class Property { Unknown, Quantity };

  rts::AbilityInstanceIndex toAbilityIndex(const std::string& s) {
    if (s == "move")
      return test::MoveAbilityIndex;
    if (s == "gather")
      return test::GatherAbilityIndex;
    return rts::AbilityInstanceIndex::None;
  }

  rts::UnitTypeId toUnitTypeId(const std::string& s) {
    if (s == "worker")
      return test::workerTypeId;
    return {};
  }

  Property toProperty(const std::string& s) {
    if (s == "quantity")
      return Property::Quantity;
    return Property::Unknown;
  }
}

namespace test::seq {
  namespace {
    using namespace std::chrono_literals;

    struct Runner {
      rts::World& world;
      Engine& engine;
      rts::SideId side;
      const Sequence& input;
      Sequence& output;

      std::map<rts::UnitId, char> unitName;
      std::map<char, rts::UnitId> unitByName;
      bool haveMap{false};

      void operator()();

    private:
      void operator()(const item::Error& e);
      void operator()(const item::Comment& e);
      void operator()(const item::Definition& d);
      void operator()(const item::Assignment& a);
      void operator()(const item::Map& m);
      void operator()(const item::Run& r);
      void operator()(const item::Command& c);

      void error(const Item& i, const std::string& s);
      item::Map makeMap();
      void command(const item::cmd::Select& s);
      void command(const item::cmd::Trigger& t);
    };

    void Runner::operator()() {
      for (const Item& item : input)
        std::visit([this](const auto& i) { (*this)(i); }, item);
    }

    void Runner::operator()(const item::Error& e) { output.push_back(e); }
    void Runner::operator()(const item::Comment& c) { output.push_back(c); }

    void Runner::operator()(const item::Definition& d) {
      auto type{toUnitTypeId(d.type)};
      if (!type)
        return error(d, "invalid unit type");
      if (d.name.size() != 1)
        return error(d, "name size must be 1");
      char name{d.name.front()};
      if (unitByName.find(name) != unitByName.end())
        return error(d, "duplicate definition");
      auto u{world.createUnit(type, side)};
      unitName[u] = name;
      unitByName[name] = u;
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

      // 1 game unit per frame
      engine.targetFps(100);
      FakeClock::step = 100us;

      haveMap = true;
      output.push_back(makeMap());
    }

    void Runner::operator()(const item::Run& r) {
      if (!haveMap)
        return error(r, "map not initialized");
      if (r.untilIdle.size() != 1)
        return error(r, "name size must be 1");
      auto it{unitByName.find(r.untilIdle.front())};
      if (it == unitByName.end())
        return error(r, "undefined name: " + r.untilIdle);
      rts::UnitStableRef u{world[it->second]};

      auto map{makeMap()};
      output.push_back(r);
      output.push_back(map);

      while (rts::Unit::nextStepTime(u) != rts::GameTimeInf) {
        engine.advanceFrame();
        auto newMap{makeMap()};
        if (newMap.map != map.map || newMap.gas != map.gas) {
          map = std::move(newMap);
          output.push_back(map);
        }
      }
    }

    void Runner::operator()(const item::Command& c) {
      if (!haveMap)
        return error(c, "map not initialized");
      std::visit([this](const auto& c) { command(c); }, c);
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
      m.time = world.time;
      m.gas = world[side].resource(test::gasResourceId).available();
      return m;
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
      test::execCommand(world, side, rts::command::TriggerAbility{ai, t.target});
      output.push_back(t);
    }
  }
}

auto test::seq::run(const Sequence& input) -> Sequence {
  Sequence output;

  auto worldPtr{rts::World::create(std::make_unique<test::Factory>())};
  rts::World& world{*worldPtr};
  test::makeSides(world);

  FakeClock::step = 0us;
  Engine engine{world, rts::GameSpeedNormal};
  Runner{world, engine, test::side1Id, input, output}();

  return output;
}
