#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "rts/Map.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/types.h"

#include <map>
#include <string>
#include <vector>

namespace test {
  constexpr rts::AbilityId MoveAbilityId{1};
  constexpr rts::EntityTypeId SimpletonTypeId{1};
  constexpr rts::EntityTypeId BuildingTypeId{2};
  constexpr rts::SideId Side1Id{1};
  constexpr rts::SideId Side2Id{2};

  extern const std::string map;

  std::vector<rts::SideId> makeSides(rts::World& w);

  struct Ui : rts::Ui {
    static std::map<char, int> count;
    char repr;

    explicit Ui(char r) : repr{r} { ++count[repr]; }
    ~Ui() override { --count[repr]; }
  };

  inline char repr(const rts::Ui& ui) { return static_cast<const Ui&>(ui).repr; }
  inline char repr(const rts::UiUPtr& ui) { return static_cast<const Ui&>(*ui).repr; }

  extern const rts::Resource gas;

  struct Factory {
    static rts::EntityId simpleton(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::EntityId building(rts::World& w, rts::Point p, rts::SideId sd);
    static rts::ResourceFieldId geyser(rts::World& w, rts::Point p);
    static rts::BlockerId rock(rts::World& w, rts::Point p);
  };

  class MapInitializer : public rts::MapInitializer {
  public:
    void operator()(rts::World& w, rts::Point p, char c) const final;
  };
}
