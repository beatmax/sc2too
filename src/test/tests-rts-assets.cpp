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
rts::ResourceId test::mineralResourceId;
rts::ResourceId test::supplyResourceId;
rts::AbilityId test::moveAbilityId;
rts::AbilityId test::buildAbilityId;
rts::AbilityId test::gatherAbilityId;
rts::AbilityId test::produceWorkerAbilityId;
rts::AbilityId test::produceThirdyAbilityId;
rts::AbilityId test::setRallyPointAbilityId;
rts::UnitTypeId test::baseTypeId;
rts::UnitTypeId test::extractorTypeId;
rts::UnitTypeId test::workerTypeId;
rts::UnitTypeId test::thirdyTypeId;
rts::SideId test::side1Id;
rts::SideId test::side2Id;

std::map<std::string, int> test::Ui::count;

void test::Factory::init(rts::World& w) {
  using RC = rts::RelativeContent;

  gasResourceId = w.createResource(std::make_unique<GasUi>());
  mineralResourceId = w.createResource(std::make_unique<MineralUi>());
  supplyResourceId = w.createResource(std::make_unique<SupplyUi>());

  moveAbilityId = w.createAbility(std::make_unique<Ui>("move"));
  buildAbilityId = w.createAbility(std::make_unique<Ui>("build"));
  gatherAbilityId = w.createAbility(std::make_unique<Ui>("gather"));

  produceWorkerAbilityId = w.createAbility(std::make_unique<Ui>("p.w"));
  produceThirdyAbilityId = w.createAbility(std::make_unique<Ui>("p.t"));
  setRallyPointAbilityId = w.createAbility(std::make_unique<Ui>("p.r"));

  baseTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, BaseMineralCost}},
      rts::ResourceQuantityList{{supplyResourceId, BaseSupplyProvision}}, BaseBuildTime,
      std::make_unique<Ui>("B"));
  extractorTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, ExtractorMineralCost}},
      rts::ResourceQuantityList{}, ExtractorBuildTime, std::make_unique<Ui>("E"),
      rts::UnitType::RequiresPower::No, 0, gasResourceId);
  workerTypeId = w.createUnitType(
      rts::UnitType::Kind::Worker,
      rts::ResourceQuantityList{
          {mineralResourceId, WorkerMineralCost}, {supplyResourceId, WorkerSupplyCost}},
      rts::ResourceQuantityList{}, WorkerBuildTime, std::make_unique<Ui>("W"));
  thirdyTypeId = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {mineralResourceId, ThirdyMineralCost}, {supplyResourceId, ThirdySupplyCost}},
      rts::ResourceQuantityList{}, ThirdyBuildTime, std::make_unique<Ui>("T"));

  w.unitTypes[baseTypeId].addAbility(
      ProduceWorkerAbilityIndex, rts::abilities::Produce{produceWorkerAbilityId, workerTypeId});
  w.unitTypes[baseTypeId].addAbility(
      ProduceThirdyAbilityIndex, rts::abilities::Produce{produceThirdyAbilityId, thirdyTypeId});
  w.unitTypes[baseTypeId].addAbility(
      SetRallyPointAbilityIndex, rts::abilities::SetRallyPoint{setRallyPointAbilityId});

  w.unitTypes[workerTypeId].addAbility(
      MoveAbilityIndex,
      rts::abilities::Move{moveAbilityId, rts::CellDistance / rts::GameTimeSecond});
  w.unitTypes[workerTypeId].addAbility(
      BuildBaseAbilityIndex, rts::abilities::Build{buildAbilityId, baseTypeId});
  w.unitTypes[workerTypeId].addAbility(
      BuildExtractorAbilityIndex, rts::abilities::Build{buildAbilityId, extractorTypeId});
  w.unitTypes[workerTypeId].addAbility(
      GatherAbilityIndex,
      rts::abilities::Gather{moveAbilityId, baseTypeId, rts::GameTimeSecond, 1});
  w.unitTypes[workerTypeId].defaultAbility[uint32_t(RC::Ground)] = MoveAbilityIndex;
  w.unitTypes[workerTypeId].defaultAbility[uint32_t(RC::Resource)] = GatherAbilityIndex;
  w.unitTypes[workerTypeId].defaultAbility[uint32_t(RC::FriendResource)] = GatherAbilityIndex;
}

rts::UnitId test::Factory::create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) {
  if (t == baseTypeId)
    return base(w, sd);
  if (t == extractorTypeId)
    return extractor(w, sd);
  if (t == workerTypeId)
    return worker(w, sd);
  if (t == thirdyTypeId)
    return thirdy(w, sd);
  return {};
}

rts::UnitId test::Factory::base(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{2, 3}, baseTypeId, sd, std::make_unique<Ui>("b"), 0, w.createProductionQueue(sd));
}

rts::UnitId test::Factory::extractor(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{2, 2}, extractorTypeId, sd, std::make_unique<Ui>("e"));
}

rts::UnitId test::Factory::worker(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{1, 1}, workerTypeId, sd, std::make_unique<Ui>("w"), WorkerCargoCapacity);
}

rts::UnitId test::Factory::thirdy(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{1, 1}, thirdyTypeId, sd, std::make_unique<Ui>("t"));
}

rts::ResourceFieldId test::Factory::geyser(rts::World& w) {
  return w.resourceFields.emplace(
      rts::Vector{2, 2}, gasResourceId, 100, rts::ResourceField::DestroyWhenEmpty::No,
      rts::ResourceField::RequiresBuilding::Yes, std::make_unique<Ui>("g"));
}

rts::ResourceFieldId test::Factory::mineralPatch(rts::World& w) {
  return w.resourceFields.emplace(
      rts::Vector{2, 2}, mineralResourceId, 100, rts::ResourceField::DestroyWhenEmpty::Yes,
      rts::ResourceField::RequiresBuilding::No, std::make_unique<Ui>("m"));
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
      return Factory::base(w, side1Id);
    case 'g':
      return Factory::geyser(w);
    case 'm':
      return Factory::mineralPatch(w);
    case 'r':
      return Factory::rock(w);
    default:
      return rts::Cell::Empty{};
  }
}

void test::makeSides(rts::World& w) {
  const rts::ResourceInitList resources{
      {mineralResourceId, 1000, rts::QuantityInf, rts::ResourceRecoverable::No},
      {supplyResourceId, 0, MaxSupplySlots, rts::ResourceRecoverable::Yes}};
  side1Id = w.createSide(resources, std::make_unique<Ui>("1"));
  side2Id = w.createSide(resources, std::make_unique<Ui>("2"));
}
