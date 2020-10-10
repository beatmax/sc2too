#pragma once

#include "Resource.h"
#include "Semaphore.h"
#include "WorldObject.h"
#include "types.h"

#include <utility>

namespace rts {
  class ResourceField : public WorldObject {
  public:
    ResourceFlexBag bag;
    ResourceGroupId group;
    Semaphore sem{1};

    ResourceField(Vector s, ResourceId r, Quantity q, UiUPtr ui, ResourceGroupId g = 0)
      : WorldObject{{-1, -1}, s, std::move(ui)}, bag{r, q, q}, group{g} {}
  };
}
