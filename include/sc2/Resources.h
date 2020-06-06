#pragma once

#include "rts/Resource.h"

namespace sc2 {
  struct Resources {
    static rts::ResourceCPtr mineral();
    static rts::ResourceCPtr gas();
    static rts::ResourceCPtr supply();

    static rts::ResourceMap initialResources();
  };
}
