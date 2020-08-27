#include "rts/Resource.h"

#include <algorithm>

rts::Quantity rts::ResourceBag::transferTo(ResourceBag& other, Quantity q) {
  q = std::min({q, quantity_, other.capacityLeft()});
  quantity_ -= q;
  other.quantity_ += q;
  return q;
}

rts::Quantity rts::ResourceBag::transferAllTo(ResourceBag& other) {
  return transferTo(other, quantity_);
}

rts::Quantity rts::ResourceFlexBag::transferAllTo(ResourceFlexBag& other) {
  if (other.resource != resource) {
    other.resource = resource;
    other.quantity_ = 0;
  }
  return ResourceBag::transferAllTo(other);
}

rts::Quantity rts::ResourceFlexBag::transferAllTo(ResourceBank& bank) {
  return ResourceBag::transferAllTo(bank[resource].available_);
}

rts::ResourceAccount::ResourceAccount(const ResourceInitDesc& d)
  : available_{d.available},
    allocated_{0, d.maxSlots},
    recoverable_{d.recoverable == ResourceRecoverable::Yes} {
}

void rts::ResourceAccount::provision(Quantity q) {
  ResourceBag{q}.transferAllTo(available_);
}

void rts::ResourceAccount::allocateTo(ResourceBag& bag, Quantity q) {
  available_.quantity_ -= q;
  allocated_.quantity_ += q;
  bag.quantity_ += q;
}

void rts::ResourceAccount::deallocate(Quantity q) {
  auto& dest{recoverable_ ? available_ : lost_};
  allocated_.transferTo(dest, q);
}

void rts::ResourceAccount::deallocateFrom(ResourceBag& bag) {
  deallocate(bag.quantity());
  ResourceBag sink;
  bag.transferAllTo(sink);
}

void rts::ResourceAccount::restoreFrom(ResourceBag& bag, Quantity q) {
  allocated_.transferTo(available_, q);
  ResourceBag sink;
  bag.transferTo(sink, q);
}

rts::ResourceBank::ResourceBank(const ResourceInitList& init) {
  for (const auto& d : init) {
    accounts_[d.resource] = ResourceAccount{d};
    maxId_ = std::max(maxId_, d.resource);
  }
}

void rts::ResourceBank::provision(const ResourceQuantityList& quantities) {
  for (auto& [r, q] : quantities)
    accounts_[r].provision(q);
}

void rts::ResourceBank::deprovision(const ResourceQuantityList& quantities, AllocFilter filter) {
  for (auto& [r, q] : quantities) {
    auto& a{accounts_[r]};
    if (a.match(filter))
      a.provision(-q);
  }
}

rts::AllocResult rts::ResourceBank::allocate(
    const ResourceQuantityList& quantities, AllocFilter filter, bool negativeOk) {
  ResourceBagArray bags;
  return allocateTo(bags, quantities, filter, negativeOk);
}

rts::AllocResult rts::ResourceBank::allocateTo(
    ResourceBagArray& bags,
    const ResourceQuantityList& quantities,
    AllocFilter filter,
    bool negativeOk) {
  if (!negativeOk) {
    for (auto& [r, q] : quantities) {
      const auto& a{accounts_[r]};
      if (a.match(filter)) {
        if (a.available() < q)
          return {AllocResult::Lack, r};
        if (a.freeSlots() < q)
          return {AllocResult::Cap, r};
      }
    }
  }
  for (auto& [r, q] : quantities) {
    const auto& a{accounts_[r]};
    if (a.match(filter))
      accounts_[r].allocateTo(bags[r], q);
  }
  return {AllocResult::Success, {}};
}

void rts::ResourceBank::deallocate(const ResourceQuantityList& quantities) {
  for (auto& [r, q] : quantities)
    accounts_[r].deallocate(q);
}

void rts::ResourceBank::deallocateFrom(ResourceBagArray& bags) {
  for (auto r{minId()}; r <= maxId(); ++r)
    accounts_[r].deallocateFrom(bags[r]);
}

void rts::ResourceBank::restoreFrom(
    ResourceBagArray& bags, const ResourceQuantityList& quantities) {
  for (auto& [r, q] : quantities)
    accounts_[r].restoreFrom(bags[r], q);
}
