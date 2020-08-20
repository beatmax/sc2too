#pragma once

#include "rts/Resource.h"

namespace ui {
  class Icon;

  struct ResourceUi : rts::ResourceUi {
    virtual const Icon& icon() const = 0;
  };

  inline const Icon& getIcon(const rts::Resource& r) {
    return static_cast<const ResourceUi&>(r.ui()).icon();
  }
}
