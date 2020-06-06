#include "tests-rts-assets.h"

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

const rts::ResourceMap test::resources{{&gas, 0}};

test::Simpleton::Simpleton(rts::Point p) : Inherited{p, rts::Vector{1, 1}, 's'} {
  addAbility(rts::abilities::move());
}

test::Building::Building(rts::Point p) : Inherited{p, rts::Vector{2, 3}, 'b'} {
}

test::Geyser::Geyser(rts::Point p) : Inherited{p, rts::Vector{2, 2}, &gas, 100, 'g'} {
}

test::Rock::Rock(rts::Point p) : Inherited{p, rts::Vector{1, 1}, 'r'} {
}

rts::Map::Cell test::CellCreator::operator()(char c, rts::Point p) const {
  switch (c) {
    case 'b':
      return Building::create(p);
    case 'g':
      return Geyser::create(p);
    case 'r':
      return Rock::create(p);
    default:
      return rts::Map::Free{};
  }
}
