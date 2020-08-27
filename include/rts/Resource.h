#pragma once

#include "constants.h"
#include "types.h"

#include <algorithm>
#include <memory>
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

  enum class ResourceRecoverable { No, Yes };

  struct ResourceInitDesc {
    ResourceId resource;
    Quantity available;
    Quantity maxSlots;
    ResourceRecoverable recoverable;
  };

  using ResourceInitList = std::vector<ResourceInitDesc>;

  class ResourceBag {
  public:
    ResourceBag(Quantity q = 0, Quantity capacity = QuantityInf)
      : quantity_{q}, capacity_{capacity} {}

    Quantity quantity() const { return quantity_; }
    Quantity capacity() const { return capacity_; }
    Quantity capacityLeft() const {
      return (capacity_ == QuantityInf) ? QuantityInf : capacity_ - quantity_;
    }
    bool empty() const { return quantity_ == 0; }
    bool full() const { return quantity_ == capacity_; }

    Quantity transferTo(ResourceBag& other, Quantity q);
    Quantity transferAllTo(ResourceBag& other);

  private:
    friend class ResourceFlexBag;
    friend class ResourceAccount;

    Quantity quantity_;
    Quantity capacity_;
  };

  struct ResourceFlexBag : ResourceBag {
    ResourceId resource;

    Quantity transferAllTo(ResourceFlexBag& other);
    Quantity transferAllTo(ResourceBank& bank);

    ResourceFlexBag(ResourceId r = {}, Quantity q = 0, Quantity capacity = QuantityInf)
      : ResourceBag{q, capacity}, resource{r} {}
  };

  using ResourceBagArray = IdIndexedArray<ResourceId, ResourceBag, MaxResources>;

  struct AllocResult {
    enum { Success, Lack, Cap } result;
    ResourceId failedResource;
  };

  enum class AllocFilter { Any, Recoverable, Unrecoverable };

  class ResourceAccount {
  public:
    ResourceAccount() = default;
    explicit ResourceAccount(const ResourceInitDesc& d);

    Quantity available() const { return available_.quantity(); }
    Quantity allocated() const { return allocated_.quantity(); }
    Quantity freeSlots() const { return std::min(available(), allocated_.capacityLeft()); }
    Quantity totalSlots() const { return allocated() + freeSlots(); }
    Quantity lost() const { return lost_.quantity(); }
    bool recoverable() const { return recoverable_; }
    bool match(AllocFilter f) const {
      return f == AllocFilter::Any || (f == AllocFilter::Recoverable) == recoverable_;
    }

    void provision(Quantity q);
    void allocateTo(ResourceBag& bag, Quantity q);
    void deallocate(Quantity q);
    void deallocateFrom(ResourceBag& bag);
    void restoreFrom(ResourceBag& bag, Quantity q);

  private:
    friend class ResourceFlexBag;

    ResourceBag available_;
    ResourceBag allocated_;
    ResourceBag lost_;
    bool recoverable_;
  };

  using ResourceAccountArray = IdIndexedArray<ResourceId, ResourceAccount, MaxResources>;

  class ResourceBank {
  public:
    explicit ResourceBank(const ResourceInitList& init);

    ResourceAccount& operator[](ResourceId r) { return accounts_[r]; }
    const ResourceAccount& operator[](ResourceId r) const { return accounts_[r]; }

    constexpr ResourceId minId() const { return ResourceId::Min; }
    ResourceId maxId() const { return maxId_; }

    void provision(const ResourceQuantityList& quantities);
    void deprovision(const ResourceQuantityList& quantities, AllocFilter filter = AllocFilter::Any);
    AllocResult allocate(
        const ResourceQuantityList& quantities,
        AllocFilter filter = AllocFilter::Any,
        bool negativeOk = false);
    AllocResult allocateTo(
        ResourceBagArray& bags,
        const ResourceQuantityList& quantities,
        AllocFilter filter = AllocFilter::Any,
        bool negativeOk = false);
    void deallocate(const ResourceQuantityList& quantities);
    void deallocateFrom(ResourceBagArray& bags);
    void restoreFrom(ResourceBagArray& bags, const ResourceQuantityList& quantities);

  private:
    ResourceAccountArray accounts_;
    ResourceId maxId_{};
  };

  struct ResourceUi : Ui {
    virtual const char* msgMoreRequired() const = 0;
    virtual const char* msgCapReached() const = 0;
  };
}
