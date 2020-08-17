#include "rts/Resource.h"

#include <algorithm>

rts::Quantity rts::ResourceBag::transferTo(ResourceBag& other, Quantity q) {
  other.resource_ = resource_;
  q = std::min({q, quantity_, other.capacityLeft()});
  quantity_ -= q;
  other.quantity_ += q;
  return q;
}

bool rts::ResourceBank::tryTransferTo(ResourceBank& other, const ResourceQuantityList& quantities) {
  for (auto& [r, q] : quantities) {
    if ((*this)[r].quantity() < q)
      return false;
  }
  for (auto& [r, q] : quantities)
    (*this)[r].transferTo(other.getOrCreate(r), q);
  return true;
}
