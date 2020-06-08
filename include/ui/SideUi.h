#pragma once

#include "rts/Side.h"

namespace ui {
  struct SideUi : rts::Ui {
    int color;

    explicit SideUi(int sideId);
  };

  inline int getColor(rts::SideCPtr s) { return static_cast<const SideUi&>(s->ui()).color; }
}
