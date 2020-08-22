#include "tests-rts-assets.h"

#include "rts/Side.h"
#include "rts/World.h"
#include "rts/constants.h"

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

std::vector<rts::SideId> test::makeSides(rts::World& w) {
  std::vector<rts::SideId> sides;
  const rts::ResourceBank resources{{&gas, 1000, rts::QuantityInf}};
  sides.push_back(w.createSide(resources, std::make_unique<Ui>('1')));
  sides.push_back(w.createSide(resources, std::make_unique<Ui>('2')));
  return sides;
}

std::map<char, int> test::Ui::count;

const rts::Resource test::gas{std::make_unique<GasUi>()};

rts::UnitId test::Factory::create(rts::World& w, rts::UnitTypeId t, rts::Point p, rts::SideId sd) {
  if (t == BuildingTypeId)
    return building(w, p, sd);
  if (t == SimpletonTypeId)
    return simpleton(w, p, sd);
  if (t == ThirdyTypeId)
    return thirdy(w, p, sd);
  return {};
}

rts::UnitId test::Factory::building(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.createUnit(
      p, rts::Vector{2, 3}, BuildingTypeId, sd, std::make_unique<Ui>('b'), 0,
      w.createProductionQueue(sd));
}

rts::UnitId test::Factory::simpleton(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.createUnit(p, rts::Vector{1, 1}, SimpletonTypeId, sd, std::make_unique<Ui>('s'));
}

rts::UnitId test::Factory::thirdy(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.createUnit(p, rts::Vector{1, 1}, ThirdyTypeId, sd, std::make_unique<Ui>('t'));
}

rts::ResourceFieldId test::Factory::geyser(rts::World& w, rts::Point p) {
  return w.createResourceField(p, rts::Vector{2, 2}, &gas, 100, std::make_unique<Ui>('g'));
}

rts::BlockerId test::Factory::rock(rts::World& w, rts::Point p) {
  return w.createBlocker(p, rts::Vector{1, 1}, std::make_unique<Ui>('r'));
}

void test::MapInitializer::operator()(rts::World& w, rts::Point p, char c) const {
  switch (c) {
    case 'b':
      Factory::building(w, p, Side1Id);
      break;
    case 'g':
      Factory::geyser(w, p);
      break;
    case 'r':
      Factory::rock(w, p);
      break;
  }
}
