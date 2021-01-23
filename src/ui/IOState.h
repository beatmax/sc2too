#pragma once

#include "Menu.h"
#include "XPixelTr.h"
#include "rts/types.h"
#include "ui/types.h"

namespace ui {
  struct IOState {
    Menu menu;

    Window headerWin;
    Window renderWin;
    Window controlWin;

    bool quit{false};
    rts::Point clickedTarget{-1, -1};
    rts::Point mousePosition{-1, -1};
    unsigned clicks{0};
    unsigned mouseButtons{0};
    rts::GeoCache geoCache;
    XPixelTr pixelTr;

    bool paused() const { return menu.active(); }
  };
}
