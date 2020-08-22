#pragma once

#include "rts/Blocker.h"
#include "rts/Factory.h"
#include "rts/Map.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/Unit.h"
#include "rts/types.h"

#include <map>
#include <string>

namespace test {
  constexpr rts::AbilityInstanceIndex MoveAbilityIndex{0};
  constexpr rts::AbilityInstanceIndex ProduceSimpletonAbilityIndex{0};
  constexpr rts::AbilityInstanceIndex ProduceThirdyAbilityIndex{1};

  constexpr rts::Quantity SimpletonCost{10};
  constexpr rts::GameTime SimpletonBuildTime{100};
  constexpr rts::Quantity ThirdyCost{2};
  constexpr rts::GameTime ThirdyBuildTime{20};

  extern const std::string map;
  extern const rts::Resource gas;

  extern rts::AbilityId moveAbilityId;
  extern rts::AbilityId produceSimpletonAbilityId;
  extern rts::AbilityId produceThirdyAbilityId;
  extern rts::UnitTypeId buildingTypeId;
  extern rts::UnitTypeId simpletonTypeId;
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
  };

  struct Factory : rts::Factory {
    void init(rts::World& w) final;
    rts::UnitId create(rts::World& w, rts::UnitTypeId t, rts::Point p, rts::SideId sd) final;
    static rts::UnitId building(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::UnitId simpleton(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::UnitId thirdy(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::ResourceFieldId geyser(rts::World& w, rts::Point p);
    static rts::BlockerId rock(rts::World& w, rts::Point p);
  };

  class MapInitializer : public rts::MapInitializer {
  public:
    void operator()(rts::World& w, rts::Point p, char c) const final;
  };

  void makeSides(rts::World& w);
}
