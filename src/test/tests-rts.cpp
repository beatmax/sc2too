#include "catch2/catch.hpp"
#include "rts/World.h"
#include "tests-rts-assets.h"

#include <sstream>

using namespace Catch::Matchers;

TEST_CASE("Hello world!", "[rts]") {
  rts::World world;
  const rts::World& cworld{world};

  SECTION("A map is loaded") {
    {
      std::istringstream is{test::map};
      world.map.load(is);
    }

    REQUIRE(cworld.map.maxX == 40);
    REQUIRE(cworld.map.maxY == 10);
    REQUIRE(isFree(cworld.map.at(9, 1)));
    REQUIRE(hasBlocker(cworld.map.at(10, 1)));
    REQUIRE(hasBlocker(cworld.map.at(11, 1)));
    REQUIRE(hasBlocker(cworld.map.at(12, 1)));
    REQUIRE(getBlocker(cworld.map.at(12, 1)) == rts::Blocker::Rock);
    REQUIRE(isFree(cworld.map.at(13, 1)));
  }
}
