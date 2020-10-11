#pragma once

#include "Group.h"
#include "abilities.h"
#include "constants.h"

#include <array>
#include <vector>

namespace rts {
  class Selection : public Group {
  public:
    using AbilityArray = std::array<const abilities::Instance*, MaxUnitAbilities>;
    using DefaultAbilityArray = std::array<AbilityInstanceIndex, uint32_t(RelativeContent::Count)>;

    struct Subgroup {
      UnitTypeId type;
      AbilityArray abilities;
      DefaultAbilityArray defaultAbility;

      UnitIdList allIds;  // not only in subgroup

      bool abilityEnabled(const World& w, AbilityInstanceIndex ai) const;
      std::vector<bool> abilityEnabled(const World& w) const;
    };

    void set(const World& w, UnitIdList ids);
    void add(const World& w, UnitIdList ids);
    void remove(const World& w, UnitIdList ids);

    const Subgroup& subgroup(const World& w) const;

    void subgroupNext(const World& w);
    void subgroupPrevious(const World& w);

  private:
    void subgroupReset(const World& w);
    void setSubgroup(const World& w, UnitTypeId type);

    mutable Subgroup subgroup_;
  };
}
