#pragma once

#include "Resource.h"
#include "types.h"

#include <cassert>
#include <utility>

namespace rts {
  class Side {
  public:
    explicit Side(ResourceMap resources, UiUPtr ui)
      : resources_{std::move(resources)}, ui_{std::move(ui)} {}

    const ResourceMap& resources() const { return resources_; }
    Quantity quantity(ResourceCPtr r) const {
      auto it = resources_.find(r);
      assert(it != resources_.end());
      return it->second;
    }
    const Ui& ui() const { return *ui_; }

  private:
    ResourceMap resources_;
    UiUPtr ui_;
  };
}
