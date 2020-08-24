#pragma once

#include "constants.h"
#include "types.h"

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace rts {
  class ResourceUi;
  using ResourceUiUPtr = std::unique_ptr<ResourceUi>;

  class Resource {
  public:
    explicit Resource(ResourceUiUPtr ui) : ui_{std::move(ui)} {}
    const ResourceUi& ui() const { return *ui_; }

  private:
    ResourceUiUPtr ui_;
  };

  class ResourceBag {
  public:
    ResourceBag(ResourceId r, Quantity q, Quantity capacity)
      : resource_{r}, quantity_{q}, capacity_{capacity} {}

    ResourceId resource() const { return resource_; }
    Quantity quantity() const { return quantity_; }
    Quantity capacity() const { return capacity_; }
    Quantity capacityLeft() const { return capacity_ - quantity_; }
    bool empty() const { return quantity_ == 0; }
    bool full() const { return quantity_ == capacity_; }
    Quantity transferTo(ResourceBag& other, Quantity q);
    Quantity transferAllTo(ResourceBag& other) { return transferTo(other, quantity_); }

  private:
    ResourceId resource_;
    Quantity quantity_;
    Quantity capacity_;
  };

  struct ResourceBank : std::vector<ResourceBag> {
    using Inherited = std::vector<ResourceBag>;

    using Inherited::Inherited;

    ResourceBag& operator[](ResourceId r) {
      auto it = std::find_if(begin(), end(), [r](const auto& b) { return b.resource() == r; });
      assert(it != end());
      return *it;
    }
    const ResourceBag& operator[](ResourceId r) const {
      return const_cast<ResourceBank&>(*this)[r];
    }

    bool depleted() const {
      return std::all_of(begin(), end(), [](const auto& b) { return b.empty(); });
    }

    std::pair<bool, ResourceId> tryTransferTo(
        ResourceBank& other, const ResourceQuantityList& quantities);

  private:
    ResourceBag& getOrCreate(ResourceId r) {
      auto it = std::find_if(begin(), end(), [r](const auto& b) { return b.resource() == r; });
      if (it == end())
        it = insert(end(), {r, 0, QuantityInf});
      return *it;
    }
  };

  struct ResourceUi : Ui {
    virtual const char* msgMoreRequired() const = 0;
  };
}
