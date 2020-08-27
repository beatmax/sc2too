#pragma once

#include "rts/Resource.h"

namespace ui {
  class Icon;

  struct ResourceUi : rts::ResourceUi {
    enum class Display { Available, Allocated };

    Display display;

    explicit ResourceUi(Display d = Display::Available) : display{d} {}
    virtual const Icon& icon() const = 0;
  };

  inline const ResourceUi& getUi(const rts::Resource& r) {
    return static_cast<const ResourceUi&>(r.ui());
  }
}
