#pragma once

#include "Resource.h"
#include "Semaphore.h"
#include "WorldObject.h"
#include "types.h"

#include <utility>

namespace rts {
  class ResourceField : public WorldObject {
  public:
    enum class DestroyWhenEmpty { No, Yes };
    enum class RequiresBuilding { No, Yes };

    ResourceFlexBag bag;
    ResourceGroupId group;
    DestroyWhenEmpty destroyWhenEmpty;
    RequiresBuilding requiresBuilding;
    Count optimalWorkerCount;
    Semaphore sem{1};

    ResourceField(
        Vector s,
        ResourceId r,
        Quantity q,
        DestroyWhenEmpty dwe,
        RequiresBuilding rb,
        Count owc,
        UiUPtr ui,
        ResourceGroupId g = 0)
      : WorldObject{{-1, -1}, s, std::move(ui)},
        bag{r, q, q},
        group{g},
        destroyWhenEmpty{dwe},
        requiresBuilding{rb},
        optimalWorkerCount{owc} {}
  };
}
