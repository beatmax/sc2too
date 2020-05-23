#include "catch2/catch.hpp"
#include "rts/World.h"
#include "tests-rts-assets.h"

using namespace Catch::Matchers;
using namespace rts;

TEST_CASE("Hello world!", "[rts]") {
  World world{Map{test::map}};
  const World& cworld{world};

  REQUIRE(cworld.map.maxX == 40);
  REQUIRE(cworld.map.maxY == 10);
  REQUIRE(isFree(cworld.map.at(9, 1)));
  REQUIRE(hasBlocker(cworld.map.at(10, 1)));
  REQUIRE(hasBlocker(cworld.map.at(11, 1)));
  REQUIRE(hasBlocker(cworld.map.at(12, 1)));
  REQUIRE(getBlocker(cworld.map.at(12, 1)) == Blocker::Rock);
  REQUIRE(isFree(cworld.map.at(13, 1)));

  SECTION("An entity is added to the world") {
    Position pos{20, 5};
    world.add(test::Simpleton::create(pos));
    REQUIRE(hasEntity(cworld.map.at(pos)));
    EntitySCPtr ce{getEntity(cworld.map.at(pos))};
    REQUIRE(ce->position == pos);

    SECTION("The entity is destroyed") {
      world.destroy(ce);
      REQUIRE(isFree(cworld.map.at(ce->position)));
      REQUIRE(ce.use_count() == 1);
    }

    SECTION("The 'move' ability is triggered") {
      EntitySPtr e{getEntity(world.map.at(pos))};
      REQUIRE(e->abilities.size() > 0);
      Ability& moveAbility = e->abilities.front();
      REQUIRE(moveAbility.name() == "move");

      world.time = 1;
      const Position targetPos{20, 3};
      world.update(Entity::trigger(moveAbility, cworld, e, targetPos));
      REQUIRE(ce->position == pos);

      SECTION("The entity moves") {
        while (pos != targetPos) {
          ++world.time;
          world.update(Entity::step(cworld, e));

          Position prevPos{pos};
          --pos.y;
          REQUIRE(ce->position == pos);
          REQUIRE(isFree(cworld.map.at(prevPos)));
          REQUIRE(hasEntity(cworld.map.at(pos)));
          REQUIRE(moveAbility.active());
        }

        // the next step deactivates the ability
        ++world.time;
        world.update(Entity::step(cworld, e));
        REQUIRE(ce->position == targetPos);
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
}
