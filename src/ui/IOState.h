#pragma once

#include "Menu.h"
#include "XTermGeo.h"
#include "rts/types.h"
#include "ui/types.h"

namespace ui {
  struct IOState {
    Menu menu;
    XTermGeo xTermGeo;
    rts::GeoCache geoCache;

    Window headerWin;
    Window renderWin;
    Window controlWin;

    bool quit{false};
    bool redraw{false};
    rts::Point mouseMapPoint{-1, -1};
    rts::Point targetPoint{-1, -1};

    bool paused() const { return menu.active(); }
  };
}
