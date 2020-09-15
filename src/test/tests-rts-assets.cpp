#include "tests-rts-assets.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "rts/constants.h"

#include <memory>
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

rts::ResourceId test::gasResourceId;
rts::ResourceId test::supplyResourceId;
rts::AbilityId test::moveAbilityId;
rts::AbilityId test::produceSimpletonAbilityId;
rts::AbilityId test::produceThirdyAbilityId;
rts::AbilityId test::setRallyPointAbilityId;
rts::UnitTypeId test::buildingTypeId;
rts::UnitTypeId test::simpletonTypeId;
rts::UnitTypeId test::thirdyTypeId;
rts::SideId test::side1Id;
rts::SideId test::side2Id;

std::map<std::string, int> test::Ui::count;

void test::Factory::init(rts::World& w) {
  using RC = rts::RelativeContent;

  gasResourceId = w.createResource(std::make_unique<GasUi>());
  supplyResourceId = w.createResource(std::make_unique<SupplyUi>());

  moveAbilityId = w.createAbility(rts::Ability::TargetType::Any, std::make_unique<Ui>("move"));
  produceSimpletonAbilityId =
      w.createAbility(rts::Ability::TargetType::None, std::make_unique<Ui>("p.s"));
  produceThirdyAbilityId =
      w.createAbility(rts::Ability::TargetType::None, std::make_unique<Ui>("p.t"));
  setRallyPointAbilityId =
      w.createAbility(rts::Ability::TargetType::None, std::make_unique<Ui>("p.r"));

  buildingTypeId = w.createUnitType(
      rts::ResourceQuantityList{},
      rts::ResourceQuantityList{{supplyResourceId, BuildingSupplyProvision}}, rts::GameTimeSecond,
      std::make_unique<Ui>("B"));
  simpletonTypeId = w.createUnitType(
      rts::ResourceQuantityList{
          {gasResourceId, SimpletonGasCost}, {supplyResourceId, SimpletonSupplyCost}},
      rts::ResourceQuantityList{}, SimpletonBuildTime, std::make_unique<Ui>("S"));
  thirdyTypeId = w.createUnitType(
      rts::ResourceQuantityList{
          {gasResourceId, ThirdyGasCost}, {supplyResourceId, ThirdySupplyCost}},
      rts::ResourceQuantityList{}, ThirdyBuildTime, std::make_unique<Ui>("T"));

  w.unitTypes[buildingTypeId].addAbility(
      ProduceSimpletonAbilityIndex,
      rts::abilities::Produce{produceSimpletonAbilityId, simpletonTypeId});
  w.unitTypes[buildingTypeId].addAbility(
      ProduceThirdyAbilityIndex, rts::abilities::Produce{produceThirdyAbilityId, thirdyTypeId});
  w.unitTypes[buildingTypeId].addAbility(
      SetRallyPointAbilityIndex, rts::abilities::SetRallyPoint{setRallyPointAbilityId});

  w.unitTypes[simpletonTypeId].addAbility(
      MoveAbilityIndex,
      rts::abilities::Move{moveAbilityId, rts::CellDistance / rts::GameTimeSecond});
  w.unitTypes[simpletonTypeId].defaultAbility[uint32_t(RC::Ground)] = moveAbilityId;
}

rts::UnitId test::Factory::create(rts::World& w, rts::UnitTypeId t, rts::Point p, rts::SideId sd) {
  if (t == buildingTypeId)
    return building(w, p, sd);
  if (t == simpletonTypeId)
    return simpleton(w, p, sd);
  if (t == thirdyTypeId)
    return thirdy(w, p, sd);
  return {};
}

rts::UnitId test::Factory::building(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.units.emplace(
      p, rts::Vector{2, 3}, buildingTypeId, sd, std::make_unique<Ui>("b"), 0,
      w.createProductionQueue(sd));
}

rts::UnitId test::Factory::simpleton(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.units.emplace(p, rts::Vector{1, 1}, simpletonTypeId, sd, std::make_unique<Ui>("s"));
}

rts::UnitId test::Factory::thirdy(rts::World& w, rts::Point p, rts::SideId sd) {
  return w.units.emplace(p, rts::Vector{1, 1}, thirdyTypeId, sd, std::make_unique<Ui>("t"));
}

rts::ResourceFieldId test::Factory::geyser(rts::World& w, rts::Point p) {
  return w.resourceFields.emplace(
      p, rts::Vector{2, 2}, gasResourceId, 100, std::make_unique<Ui>("g"));
}

rts::BlockerId test::Factory::rock(rts::World& w, rts::Point p) {
  return w.blockers.emplace(p, rts::Vector{1, 1}, std::make_unique<Ui>("r"));
}

rts::Cell::Content test::MapInitializer::operator()(
    rts::World& w, rts::Point p, char c, const std::vector<std::string>& lines) const {
  switch (c) {
    case 'b':
      return Factory::building(w, p, side1Id);
    case 'g':
      return Factory::geyser(w, p);
    case 'r':
      return Factory::rock(w, p);
    default:
      return rts::Cell::Empty{};
  }
}

void test::makeSides(rts::World& w) {
  const rts::ResourceInitList resources{
      {gasResourceId, 1000, rts::QuantityInf, rts::ResourceRecoverable::No},
      {supplyResourceId, 0, MaxSupplySlots, rts::ResourceRecoverable::Yes}};
  side1Id = w.createSide(resources, std::make_unique<Ui>("1"));
  side2Id = w.createSide(resources, std::make_unique<Ui>("2"));
}
