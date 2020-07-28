#pragma once

#include "rts/Side.h"

namespace ui {
  struct SideUi : rts::Ui {
    int color;

    explicit SideUi(int n);
  };

  inline int getColor(const rts::Side& s) { return static_cast<const SideUi&>(s.ui()).color; }
}
