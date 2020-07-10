#include "tests-rts-assets.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "rts/dimensions.h"

#include <utility>

//   0.........10........20........30........40
const std::string test::map{
    "                              gg        \n"  // 0
    "          rrr                 gg        \n"  // 1
    "                                        \n"  // 2
    "                                        \n"  // 3
    "                                        \n"  // 4
    "                                        \n"  // 5
    "              rrrrr                  bb \n"  // 6
    "              r   r                  bb \n"  // 7
    "                  r                  bb \n"  // 8
    "                  r                     \n"  // 9
};

std::vector<rts::Side> test::makeSides() {
  std::vector<rts::Side> sides;
  const rts::ResourceMap resources{{&gas, 0}};
  sides.push_back(rts::Side{resources, std::make_unique<Ui>('0')});
  sides.push_back(rts::Side{resources, std::make_unique<Ui>('1')});
  return sides;
}

std::map<char, int> test::Ui::count;

const rts::Resource test::gas{std::make_unique<Ui>('g')};

rts::Entity test::Factory::simpleton(rts::Point p, rts::SideCPtr sd) {
  rts::Entity e{p, rts::Vector{1, 1}, sd, std::make_unique<Ui>('s')};
  e.addAbility(rts::abilities::move(rts::CellDistance / rts::GameTimeSecond));
  return e;
}

rts::Entity test::Factory::building(rts::Point p, rts::SideCPtr sd) {
  return rts::Entity{p, rts::Vector{2, 3}, sd, std::make_unique<Ui>('b')};
}

rts::ResourceField test::Factory::geyser(rts::Point p) {
  return rts::ResourceField{p, rts::Vector{2, 2}, &gas, 100, std::make_unique<Ui>('g')};
}

rts::Blocker test::Factory::rock(rts::Point p) {
  return rts::Blocker{p, rts::Vector{1, 1}, std::make_unique<Ui>('r')};
}

void test::MapInitializer::operator()(rts::World& world, rts::Point p, char c) const {
  switch (c) {
    case 'b':
      world.add(Factory::building(p, &world.sides[0]));
      break;
    case 'g':
      world.add(Factory::geyser(p));
      break;
    case 'r':
      world.add(Factory::rock(p));
      break;
  }
}
