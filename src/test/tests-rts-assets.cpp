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
rts::AbilityId test::buildAbilityId;
rts::AbilityId test::gatherAbilityId;
rts::AbilityId test::produceWorkerAbilityId;
rts::AbilityId test::produceThirdyAbilityId;
rts::AbilityId test::setRallyPointAbilityId;
rts::UnitTypeId test::buildingTypeId;
rts::UnitTypeId test::workerTypeId;
rts::UnitTypeId test::thirdyTypeId;
rts::SideId test::side1Id;
rts::SideId test::side2Id;

std::map<std::string, int> test::Ui::count;

void test::Factory::init(rts::World& w) {
  using RC = rts::RelativeContent;

  gasResourceId = w.createResource(std::make_unique<GasUi>());
  supplyResourceId = w.createResource(std::make_unique<SupplyUi>());

  moveAbilityId = w.createAbility(std::make_unique<Ui>("move"));
  buildAbilityId = w.createAbility(std::make_unique<Ui>("build"));
  gatherAbilityId = w.createAbility(std::make_unique<Ui>("gather"));

  produceWorkerAbilityId = w.createAbility(std::make_unique<Ui>("p.w"));
  produceThirdyAbilityId = w.createAbility(std::make_unique<Ui>("p.t"));
  setRallyPointAbilityId = w.createAbility(std::make_unique<Ui>("p.r"));

  buildingTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure, rts::ResourceQuantityList{{gasResourceId, BuildingGasCost}},
      rts::ResourceQuantityList{{supplyResourceId, BuildingSupplyProvision}}, BuildingBuildTime,
      std::make_unique<Ui>("B"));
  workerTypeId = w.createUnitType(
      rts::UnitType::Kind::Worker,
      rts::ResourceQuantityList{
          {gasResourceId, WorkerGasCost}, {supplyResourceId, WorkerSupplyCost}},
      rts::ResourceQuantityList{}, WorkerBuildTime, std::make_unique<Ui>("W"));
  thirdyTypeId = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {gasResourceId, ThirdyGasCost}, {supplyResourceId, ThirdySupplyCost}},
      rts::ResourceQuantityList{}, ThirdyBuildTime, std::make_unique<Ui>("T"));

  w.unitTypes[buildingTypeId].addAbility(
      ProduceWorkerAbilityIndex, rts::abilities::Produce{produceWorkerAbilityId, workerTypeId});
  w.unitTypes[buildingTypeId].addAbility(
      ProduceThirdyAbilityIndex, rts::abilities::Produce{produceThirdyAbilityId, thirdyTypeId});
  w.unitTypes[buildingTypeId].addAbility(
      SetRallyPointAbilityIndex, rts::abilities::SetRallyPoint{setRallyPointAbilityId});

  w.unitTypes[workerTypeId].addAbility(
      MoveAbilityIndex,
      rts::abilities::Move{moveAbilityId, rts::CellDistance / rts::GameTimeSecond});
  w.unitTypes[workerTypeId].addAbility(
      BuildAbilityIndex, rts::abilities::Build{buildAbilityId, buildingTypeId});
  w.unitTypes[workerTypeId].addAbility(
      GatherAbilityIndex,
      rts::abilities::Gather{moveAbilityId, buildingTypeId, rts::GameTimeSecond, 1});
  w.unitTypes[workerTypeId].defaultAbility[uint32_t(RC::Ground)] = MoveAbilityIndex;
  w.unitTypes[workerTypeId].defaultAbility[uint32_t(RC::Resource)] = GatherAbilityIndex;
}

rts::UnitId test::Factory::create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) {
  if (t == buildingTypeId)
    return building(w, sd);
  if (t == workerTypeId)
    return worker(w, sd);
  if (t == thirdyTypeId)
    return thirdy(w, sd);
  return {};
}

rts::UnitId test::Factory::building(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{2, 3}, buildingTypeId, sd, std::make_unique<Ui>("b"), 0,
      w.createProductionQueue(sd));
}

rts::UnitId test::Factory::worker(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{1, 1}, workerTypeId, sd, std::make_unique<Ui>("w"), WorkerCargoCapacity);
}

rts::UnitId test::Factory::thirdy(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{1, 1}, thirdyTypeId, sd, std::make_unique<Ui>("t"));
}

rts::ResourceFieldId test::Factory::geyser(rts::World& w) {
  return w.resourceFields.emplace(rts::Vector{2, 2}, gasResourceId, 100, std::make_unique<Ui>("g"));
}

rts::BlockerId test::Factory::rock(rts::World& w) {
  return w.blockers.emplace(rts::Vector{1, 1}, std::make_unique<Ui>("r"));
}

rts::Cell::Content test::MapInitializer::operator()(
    rts::World& w, rts::Point p, char c, const std::vector<std::string>& lines) const {
  {
    auto unitIt{predefinedUnits_.find(c)};
    if (unitIt != predefinedUnits_.end())
      return unitIt->second;
  }
  switch (c) {
    case 'b':
      return Factory::building(w, side1Id);
    case 'g':
      return Factory::geyser(w);
    case 'r':
      return Factory::rock(w);
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
