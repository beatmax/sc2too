#pragma once

#include "Resource.h"
#include "WorldObject.h"

#include <utility>

namespace rts {
  class ResourceField : public WorldObject {
  public:
    ResourceField(Point p, Vector s, ResourceCPtr r, Quantity q, UiUPtr ui)
      : WorldObject{p, s, std::move(ui)}, resource_{r}, quantity_{q} {}

    ResourceCPtr resource() const { return resource_; }
    Quantity quantity() const { return quantity_; }

  private:
    ResourceCPtr resource_;
    Quantity quantity_;
  };
}
