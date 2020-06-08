#include "tests-rts-assets.h"

#include "rts/World.h"
#include "rts/abilities.h"

#include <utility>

//   0.........10........20........30........40
const std::string test::map{
    "                              gg        \n"  // 0
    "          rrr                 gg        \n"  // 1
    "                                        \n"  // 2
    "                                        \n"  // 3
    "                                        \n"  // 4
    "                                        \n"  // 5
    "                                     bb \n"  // 6
    "                                     bb \n"  // 7
    "                                     bb \n"  // 8
    "                                        \n"  // 9
};

const rts::Resource test::gas{std::make_unique<Ui>('g')};

std::vector<rts::Side> test::makeSides() {
  std::vector<rts::Side> sides;
  const rts::ResourceMap resources{{&gas, 0}};
  sides.push_back(rts::Side{resources, std::make_unique<Ui>('0')});
  sides.push_back(rts::Side{resources, std::make_unique<Ui>('1')});
  return sides;
}

test::Simpleton::Simpleton(rts::Point p, rts::SideCPtr sd)
  : Inherited{p, rts::Vector{1, 1}, sd, 's'} {
  addAbility(rts::abilities::move());
}

test::Building::Building(rts::Point p, rts::SideCPtr sd)
  : Inherited{p, rts::Vector{2, 3}, sd, 'b'} {
}

test::Geyser::Geyser(rts::Point p) : Inherited{p, rts::Vector{2, 2}, &gas, 100, 'g'} {
}

test::Rock::Rock(rts::Point p) : Inherited{p, rts::Vector{1, 1}, 'r'} {
}

rts::Map::Cell test::CellCreator::operator()(const rts::World& world, rts::Point p, char c) const {
  switch (c) {
    case 'b':
      return Building::create(p, &world.sides[0]);
    case 'g':
      return Geyser::create(p);
    case 'r':
      return Rock::create(p);
    default:
      return rts::Map::Free{};
  }
}
