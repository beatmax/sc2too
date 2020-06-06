#pragma once

#include "Resource.h"
#include "types.h"

#include <cassert>

namespace rts {
  class Side {
  public:
    explicit Side(ResourceMap resources) : resources_{resources} {}

    const ResourceMap& resources() const { return resources_; }
    Quantity quantity(ResourceCPtr r) const {
      auto it = resources_.find(r);
      assert(it != resources_.end());
      return it->second;
    }

  private:
    ResourceMap resources_;
  };
}
