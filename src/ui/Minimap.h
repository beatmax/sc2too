#pragma once

#include "rts/types.h"
#include "ui/Frame.h"
#include "ui/dim.h"
#include "ui/types.h"

namespace ui {
  class Minimap {
  public:
    void init(const rts::World& w);
    void update(const Window& win, const rts::World& w, const Camera& camera, rts::SideId side);

  private:
    void updateFrame(
        const rts::Minimap& minimap, const rts::Rectangle& visibleArea, rts::SideId side);

    Frame frame_{dim::MinimapArea.size.y, dim::MinimapArea.size.x};
    MatrixVector cameraSize_;
  };
}
