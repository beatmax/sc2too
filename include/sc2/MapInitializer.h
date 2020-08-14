#pragma once

#include "rts/Map.h"
#include "rts/types.h"

namespace sc2 {
  class MapInitializer : public rts::MapInitializer {
  public:
    void operator()(rts::World& w, rts::Point p, char c) const final;

  private:
    rts::ResourceGroupId mineralGroup(const rts::World& w, rts::Point p) const;

    mutable rts::ResourceGroupId mineralGroupId_{0};
  };
}
