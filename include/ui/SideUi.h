#pragma once

#include "rts/Side.h"
#include "types.h"

namespace ui {
  struct SideUi : rts::Ui {
    Color color;

    explicit SideUi(int n);
  };

  inline Color getColor(const rts::Side& s) { return static_cast<const SideUi&>(s.ui()).color; }
}
