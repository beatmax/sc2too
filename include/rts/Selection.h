#pragma once

#include "Group.h"

namespace rts {
  class Selection : public Group {
  public:
    void set(const World& w, EntityIdList ids);
    void add(const World& w, EntityIdList ids);

    EntityTypeId subgroupType(const World& w) const;
    void subgroupNext(const World& w);
    void subgroupPrevious(const World& w);

  private:
    void subgroupReset(const World& w);

    mutable EntityTypeId subgroupType_;
  };
}
