#pragma once

#include "types.h"

#include <map>
#include <utility>

namespace rts {
  class Resource {
  public:
    explicit Resource(UiUPtr ui) : ui_{std::move(ui)} {}
    const Ui& ui() const { return *ui_; }

  private:
    UiUPtr ui_;
  };

  using ResourceMap = std::map<ResourceCPtr, Quantity>;
}
