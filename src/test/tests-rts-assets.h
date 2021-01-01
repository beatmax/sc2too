#pragma once

#include "rts/Factory.h"
#include "rts/Map.h"
#include "rts/Resource.h"
#include "rts/types.h"

#include <map>
#include <string>

namespace test {
  constexpr rts::Quantity MaxSupplySlots{200};

  constexpr rts::AbilityInstanceIndex MoveAbilityIndex{0};
  constexpr rts::AbilityInstanceIndex BuildBaseAbilityIndex{1};
  constexpr rts::AbilityInstanceIndex BuildExtractorAbilityIndex{2};
  constexpr rts::AbilityInstanceIndex GatherAbilityIndex{3};

  constexpr rts::AbilityInstanceIndex ProduceWorkerAbilityIndex{0};
  constexpr rts::AbilityInstanceIndex ProduceThirdyAbilityIndex{1};
  constexpr rts::AbilityInstanceIndex SetRallyPointAbilityIndex{2};

  constexpr rts::Quantity BaseMineralCost{20};
  constexpr rts::Quantity BaseSupplyProvision{15};
  constexpr rts::Quantity BaseBuildTime{30};
  constexpr rts::Quantity ExtractorMineralCost{20};
  constexpr rts::Quantity ExtractorBuildTime{10};
  constexpr rts::Quantity WorkerMineralCost{10};
  constexpr rts::Quantity WorkerSupplyCost{2};
  constexpr rts::GameTime WorkerBuildTime{100};
  constexpr rts::GameTime WorkerCargoCapacity{5};
  constexpr rts::Quantity ThirdyMineralCost{2};
  constexpr rts::Quantity ThirdySupplyCost{4};
  constexpr rts::GameTime ThirdyBuildTime{20};

  extern const std::string map;

  extern rts::ResourceId gasResourceId;
  extern rts::ResourceId mineralResourceId;
  extern rts::ResourceId supplyResourceId;
  extern rts::AbilityId moveAbilityId;
  extern rts::AbilityId buildAbilityId;
  extern rts::AbilityId gatherAbilityId;
  extern rts::AbilityId produceWorkerAbilityId;
  extern rts::AbilityId produceThirdyAbilityId;
  extern rts::AbilityId setRallyPointAbilityId;
  extern rts::UnitTypeId baseTypeId;
  extern rts::UnitTypeId extractorTypeId;
  extern rts::UnitTypeId workerTypeId;
  extern rts::UnitTypeId thirdyTypeId;
  extern rts::SideId side1Id;
  extern rts::SideId side2Id;

  struct Ui : rts::Ui {
    static std::map<std::string, int> count;
    std::string repr;

    explicit Ui(std::string r) : repr{r} { ++count[repr]; }
    ~Ui() override { --count[repr]; }
  };

  inline std::string repr(const rts::Ui& ui) { return static_cast<const Ui&>(ui).repr; }
  inline std::string repr(const rts::UiUPtr& ui) { return static_cast<const Ui&>(*ui).repr; }

  struct GasUi : rts::ResourceUi {
    const char* msgMoreRequired() const final { return "Not enough gas!"; }
    const char* msgCapReached() const final { return "Gas cap reached!"; }
  };

  struct MineralUi : rts::ResourceUi {
    const char* msgMoreRequired() const final { return "Not enough mineral!"; }
    const char* msgCapReached() const final { return "Mineral cap reached!"; }
  };

  struct SupplyUi : rts::ResourceUi {
    const char* msgMoreRequired() const final { return "Not enough supply!"; }
    const char* msgCapReached() const final { return "Supply cap reached!"; }
  };

  struct Factory : rts::Factory {
    void init(rts::World& w) final;
    rts::UnitId create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) final;
    static rts::UnitId base(rts::World& w, rts::SideId sd);
    static rts::UnitId extractor(rts::World& w, rts::SideId sd);
    static rts::UnitId worker(rts::World& w, rts::SideId sd);
    static rts::UnitId thirdy(rts::World& w, rts::SideId sd);
    static rts::ResourceFieldId geyser(rts::World& w);
    static rts::ResourceFieldId mineralPatch(rts::World& w);
    static rts::BlockerId rock(rts::World& w);
  };

  class MapInitializer : public rts::MapInitializer {
  public:
    MapInitializer() = default;
    explicit MapInitializer(const std::map<char, rts::UnitId>& units) : predefinedUnits_{units} {}

    rts::Cell::Content operator()(
        rts::World& w, rts::Point p, char c, const std::vector<std::string>& lines) const final;

  private:
    const std::map<char, rts::UnitId> predefinedUnits_;
  };

  void makeSides(rts::World& w);
}
