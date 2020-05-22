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
    EntitySCPtr e{getEntity(cworld.map.at(pos))};
    REQUIRE(e->position == pos);
  }
}
