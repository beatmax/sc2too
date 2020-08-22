#pragma once

#include "Group.h"

namespace rts {
  class Selection : public Group {
  public:
    void set(const World& w, UnitIdList ids);
    void add(const World& w, UnitIdList ids);

    UnitTypeId subgroupType(const World& w) const;
    void subgroupNext(const World& w);
    void subgroupPrevious(const World& w);

  private:
    void subgroupReset(const World& w);

    mutable UnitTypeId subgroupType_;
  };
}
