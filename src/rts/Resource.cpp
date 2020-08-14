#include "rts/Resource.h"

#include <algorithm>

rts::Quantity rts::ResourceBag::transferTo(ResourceBag& other, Quantity q) {
  other.resource_ = resource_;
  q = std::min({q, quantity_, other.capacityLeft()});
  quantity_ -= q;
  other.quantity_ += q;
  return q;
}
