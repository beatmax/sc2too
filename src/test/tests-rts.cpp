#include "catch2/catch.hpp"
#include "rts/World.h"
#include "tests-rts-assets.h"

using namespace Catch::Matchers;
using namespace rts;

TEST_CASE("Hello world!", "[rts]") {
  World world{Map{test::CellCreator{}, test::map}, 1, test::resources};
  const World& cworld{world};

  REQUIRE(cworld.map.maxX == 40);
  REQUIRE(cworld.map.maxY == 10);
  REQUIRE(isFree(cworld.map.at(9, 1)));
  REQUIRE(hasBlocker(cworld.map.at(10, 1)));
  REQUIRE(hasBlocker(cworld.map.at(11, 1)));
  REQUIRE(hasBlocker(cworld.map.at(12, 1)));
  REQUIRE(test::repr(getBlocker(cworld.map.at(12, 1)).ui()) == 'r');
  REQUIRE(isFree(cworld.map.at(13, 1)));

  const Rectangle buildingArea{Point{37, 6}, rts::Vector{2, 3}};
  REQUIRE(hasEntity(cworld.map.at(buildingArea.topLeft)));
  EntitySCPtr building{getEntity(cworld.map.at(buildingArea.topLeft))};
  REQUIRE(building->area == buildingArea);
  REQUIRE(test::repr(building->ui()) == 'b');
  forEachPoint(buildingArea, [&](Point p) {
    REQUIRE(hasEntity(cworld.map.at(p)));
    REQUIRE(getEntity(cworld.map.at(p)) == building);
  });

  const Rectangle geyserArea{Point{30, 0}, rts::Vector{2, 2}};
  REQUIRE(hasResourceField(cworld.map.at(geyserArea.topLeft)));
  ResourceFieldSCPtr geyser{getResourceField(cworld.map.at(geyserArea.topLeft))};
  REQUIRE(geyser->area == geyserArea);
  REQUIRE(test::repr(geyser->ui()) == 'g');
  forEachPoint(geyserArea, [&](Point p) {
    REQUIRE(hasResourceField(cworld.map.at(p)));
    REQUIRE(getResourceField(cworld.map.at(p)) == geyser);
  });

  REQUIRE(cworld.sides.size() == 1);
  REQUIRE(cworld.sides[0].quantity(&test::gas) == 0);

  SECTION("An entity is added to the world") {
    Point pos{20, 5};
    world.add(test::Simpleton::create(pos));
    REQUIRE(hasEntity(cworld.map.at(pos)));
    EntitySCPtr ce{getEntity(cworld.map.at(pos))};
    REQUIRE(ce->area.topLeft == pos);
    REQUIRE(test::repr(ce->ui()) == 's');

    SECTION("The entity is destroyed") {
      world.destroy(ce);
      REQUIRE(isFree(cworld.map.at(ce->area.topLeft)));
      REQUIRE(ce.use_count() == 1);
    }

    SECTION("The 'move' ability is triggered") {
      EntitySPtr e{getEntity(world.map.at(pos))};
      REQUIRE(e->abilities.size() > 0);
      Ability& moveAbility = e->abilities.front();
      REQUIRE(moveAbility.name() == "move");

      world.time = 1;
      const Point targetPos{20, 3};
      world.update(Entity::trigger(moveAbility, cworld, e, targetPos));
      REQUIRE(ce->area.topLeft == pos);

      SECTION("The entity moves") {
        while (pos != targetPos) {
          ++world.time;
          world.update(Entity::step(cworld, e));

          Point prevPos{pos};
          --pos.y;
          REQUIRE(ce->area.topLeft == pos);
          REQUIRE(isFree(cworld.map.at(prevPos)));
          REQUIRE(hasEntity(cworld.map.at(pos)));
          REQUIRE(moveAbility.active());
        }

        // the next step deactivates the ability
        ++world.time;
        world.update(Entity::step(cworld, e));
        REQUIRE(ce->area.topLeft == targetPos);
        REQUIRE(!moveAbility.active());
        REQUIRE(moveAbility.nextStepTime() == 0);
      }

      SECTION("The entity is destroyed with pending actions on it") {
        ++world.time;
        WorldActionList actions{Entity::step(cworld, e)};
        REQUIRE(!actions.empty());
        e.reset();
        REQUIRE(ce.use_count() == 2);
        world.destroy(ce);
        REQUIRE(ce.use_count() == 1);
        ce.reset();
        world.update(actions);
      }
    }
  }

  SECTION("A multi-cell entity is added to the world") {
    const Rectangle area{Point{1, 1}, rts::Vector{2, 3}};

    world.add(test::Building::create(area.topLeft));
    EntitySCPtr ce{getEntity(cworld.map.at(area.topLeft))};
    REQUIRE(ce->area == area);

    const Rectangle outRect{area.topLeft - rts::Vector{1, 1}, area.size + rts::Vector{2, 2}};
    forEachPoint(outRect, [&](Point p) {
      if (area.contains(p)) {
        REQUIRE(hasEntity(cworld.map.at(p)));
        REQUIRE(getEntity(cworld.map.at(p)) == ce);
      }
      else {
        REQUIRE(isFree(cworld.map.at(p)));
      }
    });

    SECTION("The multi-cell entity is destroyed") {
      world.destroy(ce);
      forEachPoint(area, [&](Point p) { REQUIRE(isFree(cworld.map.at(p))); });
      REQUIRE(ce.use_count() == 1);
      ce.reset();
    }
  }
}
