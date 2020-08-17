#pragma once

#include "Resource.h"
#include "Semaphore.h"
#include "WorldObject.h"
#include "types.h"

#include <utility>

namespace rts {
  class ResourceField : public WorldObject {
  public:
    ResourceBag bag;
    ResourceGroupId group;
    Semaphore sem{1};

    ResourceField(Point p, Vector s, ResourceCPtr r, Quantity q, UiUPtr ui, ResourceGroupId g = 0)
      : WorldObject{p, s, std::move(ui)}, bag{r, q, q}, group{g} {}
  };
}
