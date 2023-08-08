#pragma once

#include "rts/types.h"

#include <set>
#include <vector>

namespace sc2 {
  class BuildOrder {
  public:
    void init(const rts::World& w);
    void check(rts::World& w, rts::ProducibleId p, rts::SideId sd);

  private:
    std::vector<rts::ProducibleId> order_;
    std::set<rts::ProducibleId> ignored_;
    size_t next_{0};
  };
}
