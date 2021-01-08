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

rts::AbilityId test::buildAbilityId;
rts::AbilityId test::gatherAbilityId;
rts::AbilityId test::moveAbilityId;
rts::AbilityId test::produceFighterAbilityId;
rts::AbilityId test::produceThirdyAbilityId;
rts::AbilityId test::produceWorkerAbilityId;
rts::AbilityId test::setRallyPointAbilityId;

rts::UnitTypeId test::baseTypeId;
rts::UnitTypeId test::dojoTypeId;
rts::UnitTypeId test::extractorTypeId;
rts::UnitTypeId test::powerPlantTypeId;

rts::UnitTypeId test::fighterTypeId;
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

  buildAbilityId = w.createAbility(std::make_unique<Ui>("build"));
  gatherAbilityId = w.createAbility(std::make_unique<Ui>("gather"));
  moveAbilityId = w.createAbility(std::make_unique<Ui>("move"));
  produceFighterAbilityId = w.createAbility(std::make_unique<Ui>("p.f"));
  produceThirdyAbilityId = w.createAbility(std::make_unique<Ui>("p.t"));
  produceWorkerAbilityId = w.createAbility(std::make_unique<Ui>("p.w"));
  setRallyPointAbilityId = w.createAbility(std::make_unique<Ui>("rally"));

  baseTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, BaseMineralCost}},
      rts::ResourceQuantityList{{supplyResourceId, BaseSupplyProvision}}, BaseBuildTime,
      std::make_unique<Ui>("B"));
  dojoTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, DojoMineralCost}}, rts::ResourceQuantityList{},
      DojoBuildTime, std::make_unique<Ui>("D"), rts::UnitType::RequiresPower::Yes);
  extractorTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, ExtractorMineralCost}},
      rts::ResourceQuantityList{}, ExtractorBuildTime, std::make_unique<Ui>("E"),
      rts::UnitType::RequiresPower::No, 0, gasResourceId);
  powerPlantTypeId = w.createUnitType(
      rts::UnitType::Kind::Structure,
      rts::ResourceQuantityList{{mineralResourceId, PowerPlantMineralCost}},
      rts::ResourceQuantityList{}, PowerPlantBuildTime, std::make_unique<Ui>("P"),
      rts::UnitType::RequiresPower::No, PowerPlantPowerRadius);

  // creation order of unit types determines subgroup order in selection
  fighterTypeId = w.createUnitType(
      rts::UnitType::Kind::Army,
      rts::ResourceQuantityList{
          {mineralResourceId, FighterMineralCost}, {supplyResourceId, FighterSupplyCost}},
      rts::ResourceQuantityList{}, FighterBuildTime, std::make_unique<Ui>("F"));
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

  w[baseTypeId].addAbility(
      ProduceThirdyAbilityIndex, rts::abilities::Produce{produceThirdyAbilityId, thirdyTypeId});
  w[baseTypeId].addAbility(
      ProduceWorkerAbilityIndex, rts::abilities::Produce{produceWorkerAbilityId, workerTypeId});
  w[baseTypeId].addAbility(
      SetRallyPointAbilityIndex, rts::abilities::SetRallyPoint{setRallyPointAbilityId});

  w[dojoTypeId].addAbility(
      ProduceFighterAbilityIndex, rts::abilities::Produce{produceFighterAbilityId, fighterTypeId});
  w[dojoTypeId].addAbility(
      SetRallyPointAbilityIndex, rts::abilities::SetRallyPoint{setRallyPointAbilityId});

  w[fighterTypeId].addAbility(
      MoveAbilityIndex,
      rts::abilities::Move{moveAbilityId, rts::CellDistance / rts::GameTimeSecond});

  w[workerTypeId].addAbility(
      MoveAbilityIndex,
      rts::abilities::Move{moveAbilityId, rts::CellDistance / rts::GameTimeSecond});
  w[workerTypeId].addAbility(
      BuildBaseAbilityIndex, rts::abilities::Build{buildAbilityId, baseTypeId});
  w[workerTypeId].addAbility(
      BuildDojoAbilityIndex, rts::abilities::Build{buildAbilityId, dojoTypeId});
  w[workerTypeId].addAbility(
      BuildExtractorAbilityIndex, rts::abilities::Build{buildAbilityId, extractorTypeId});
  w[workerTypeId].addAbility(
      BuildPowerPlantAbilityIndex, rts::abilities::Build{buildAbilityId, powerPlantTypeId});
  w[workerTypeId].addAbility(
      GatherAbilityIndex, rts::abilities::Gather{moveAbilityId, rts::GameTimeSecond, 1});
  w[workerTypeId].defaultAbility[uint32_t(RC::Ground)] = MoveAbilityIndex;
  w[workerTypeId].defaultAbility[uint32_t(RC::Resource)] = GatherAbilityIndex;
  w[workerTypeId].defaultAbility[uint32_t(RC::FriendResource)] = GatherAbilityIndex;
}

rts::UnitId test::Factory::create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) {
  if (t == baseTypeId)
    return base(w, sd);
  if (t == dojoTypeId)
    return dojo(w, sd);
  if (t == extractorTypeId)
    return extractor(w, sd);
  if (t == fighterTypeId)
    return fighter(w, sd);
  if (t == powerPlantTypeId)
    return powerPlant(w, sd);
  if (t == thirdyTypeId)
    return thirdy(w, sd);
  if (t == workerTypeId)
    return worker(w, sd);
  return {};
}

rts::UnitId test::Factory::base(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{2, 3}, baseTypeId, sd, std::make_unique<Ui>("b"), 0, w.createProductionQueue(sd));
}

rts::UnitId test::Factory::dojo(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{2, 2}, dojoTypeId, sd, std::make_unique<Ui>("d"), 0, w.createProductionQueue(sd));
}

rts::UnitId test::Factory::extractor(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{2, 2}, extractorTypeId, sd, std::make_unique<Ui>("e"));
}

rts::UnitId test::Factory::fighter(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{1, 1}, fighterTypeId, sd, std::make_unique<Ui>("f"));
}

rts::UnitId test::Factory::powerPlant(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{1, 1}, powerPlantTypeId, sd, std::make_unique<Ui>("p"));
}

rts::UnitId test::Factory::thirdy(rts::World& w, rts::SideId sd) {
  return w.units.emplace(rts::Vector{1, 1}, thirdyTypeId, sd, std::make_unique<Ui>("t"));
}

rts::UnitId test::Factory::worker(rts::World& w, rts::SideId sd) {
  return w.units.emplace(
      rts::Vector{1, 1}, workerTypeId, sd, std::make_unique<Ui>("w"), WorkerCargoCapacity);
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
    auto unitIt{predefinedUnits_.find(std::string{c})};
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
  side1Id = w.createSide(baseTypeId, resources, std::make_unique<Ui>("1"));
  side2Id = w.createSide(baseTypeId, resources, std::make_unique<Ui>("2"));
}
