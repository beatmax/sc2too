#pragma once

#include "rts/Resource.h"
#include "rts/types.h"

namespace sc2 {
  struct Resources {
    static void init(rts::World& w);
    static rts::ResourceInitList initialResources();

    static rts::ResourceId mineral;
    static rts::ResourceId gas;
    static rts::ResourceId supply;
  };
}
