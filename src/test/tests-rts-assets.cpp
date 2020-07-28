#include "tests-rts-assets.h"

#include "rts/Side.h"
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

std::vector<rts::SideId> test::makeSides(rts::World& world) {
  std::vector<rts::SideId> sides;
  const rts::ResourceMap resources{{&gas, 0}};
  sides.push_back(world.createSide(resources, std::make_unique<Ui>('1')));
  sides.push_back(world.createSide(resources, std::make_unique<Ui>('2')));
  return sides;
}

std::map<char, int> test::Ui::count;

const rts::Resource test::gas{std::make_unique<Ui>('g')};

rts::EntityId test::Factory::simpleton(rts::World& w, rts::Point p, rts::SideId sd) {
  auto id{w.createEntity(p, rts::Vector{1, 1}, sd, std::make_unique<Ui>('s'))};
  auto& e{w.entities[id]};
  e.addAbility(rts::abilities::move(rts::CellDistance / rts::GameTimeSecond));
  return id;
}

rts::EntityId test::Factory::building(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.createEntity(p, rts::Vector{2, 3}, sd, std::make_unique<Ui>('b'));
}

rts::ResourceFieldId test::Factory::geyser(rts::World& w, rts::Point p) {
  return w.createResourceField(p, rts::Vector{2, 2}, &gas, 100, std::make_unique<Ui>('g'));
}

rts::BlockerId test::Factory::rock(rts::World& w, rts::Point p) {
  return w.createBlocker(p, rts::Vector{1, 1}, std::make_unique<Ui>('r'));
}

void test::MapInitializer::operator()(rts::World& world, rts::Point p, char c) const {
  switch (c) {
    case 'b':
      Factory::building(world, p, Side1Id);
      break;
    case 'g':
      Factory::geyser(world, p);
      break;
    case 'r':
      Factory::rock(world, p);
      break;
  }
}
