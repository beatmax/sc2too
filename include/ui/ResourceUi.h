#pragma once

#include "rts/Resource.h"

namespace ui {
  struct ResourceUi : rts::Ui {
    char repr;

    explicit ResourceUi(char r) : repr{r} {}
  };

  inline char repr(const rts::Resource& r) { return static_cast<const ResourceUi&>(r.ui()).repr; }
}
