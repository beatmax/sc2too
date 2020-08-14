#pragma once

#include "types.h"

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace rts {
  class Resource {
  public:
    explicit Resource(UiUPtr ui) : ui_{std::move(ui)} {}
    const Ui& ui() const { return *ui_; }

  private:
    UiUPtr ui_;
  };

  class ResourceBag {
  public:
    ResourceBag(ResourceCPtr r, Quantity q, Quantity capacity)
      : resource_{r}, quantity_{q}, capacity_{capacity} {}

    ResourceCPtr resource() const { return resource_; }
    Quantity quantity() const { return quantity_; }
    Quantity capacity() const { return capacity_; }
    Quantity capacityLeft() const { return capacity_ - quantity_; }
    bool empty() const { return quantity_ == 0; }
    bool full() const { return quantity_ == capacity_; }
    Quantity transferTo(ResourceBag& other, Quantity q);
    Quantity transferAllTo(ResourceBag& other) { return transferTo(other, quantity_); }

  private:
    ResourceCPtr resource_;
    Quantity quantity_;
    const Quantity capacity_;
  };

  struct ResourceMap : std::vector<ResourceBag> {
    using Inherited = std::vector<ResourceBag>;

    using Inherited::Inherited;

    ResourceBag& operator[](ResourceCPtr r) {
      auto it = std::find_if(begin(), end(), [r](const auto& b) { return b.resource() == r; });
      assert(it != end());
      return *it;
    }
    const ResourceBag& operator[](ResourceCPtr r) const {
      return const_cast<ResourceMap&>(*this)[r];
    }
  };
}
