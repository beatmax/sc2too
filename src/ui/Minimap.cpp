#include "Minimap.h"

#include "graph.h"
#include "rts/World.h"
#include "ui/Camera.h"
#include "ui/fx.h"

#include <cassert>

namespace ui {
  namespace {
    const std::array<Color, 256> LightenTr = [] {
      using CP = std::pair<int, int>;
      std::array<Color, 256> t;
      for (auto [k, v] : {CP{16, 233}, CP{19, 21}, CP{32, 69}, CP{160, 196}, CP{235, 238}})
        t[k] = Color(v);
      return t;
    }();

    Color colorOf(const rts::MinimapCell& c, rts::SideId side) {
      auto friends{c.sideUnits[side]};
      auto foes{c.units - friends};
      if (foes)
        return Color(160);
      if (friends)
        return Color(19);
      if (c.resourceFields)
        return Color(32);
      if (c.blockers)
        return Color(235);
      return Color(16);
    }
  }
}

void ui::Minimap::init(const rts::World& w) {
  const wchar_t wch[]{L"▀"};
  for (int y{0}; y < frame_.rows(); ++y) {
    for (int x{0}; x < frame_.cols(); ++x)
      setcchar(&frame_(y, x), wch, 0, 0, nullptr);
  }

  // fixed camera size in minimap; make it fit at bottom-right corner
  cameraSize_ = w.minimap.rect().bottomRightOut() -
      w.minimap.mapToMinimap(w.map.area().bottomRightOut() - Camera::Size);
}

void ui::Minimap::update(
    const Window& win, const rts::World& w, const Camera& camera, rts::SideId side) {
  rts::Rectangle visibleArea{w.minimap.mapToMinimap(camera.area().topLeft), cameraSize_};
  updateFrame(w.minimap, visibleArea, side);
  graph::drawFrame(win, frame_, dim::MinimapArea.topLeft);
}

void ui::Minimap::updateFrame(
    const rts::Minimap& minimap, const rts::Rectangle& visibleArea, rts::SideId side) {
  assert(minimap.size() == scale(dim::MinimapArea.size, ScreenVector{1, 2}));

  Color color[dim::MinimapArea.size.y * 2][dim::MinimapArea.size.x];
  for (auto p : minimap.rect().points()) {
    color[p.y][p.x] = colorOf(minimap[p], side);
  }
  for (auto p : visibleArea.points()) {
    auto& c{color[p.y][p.x]};
    c = LightenTr[int(c)];
  }

  for (int y{0}; y < frame_.rows(); ++y) {
    for (int x{0}; x < frame_.cols(); ++x)
      frame_(y, x).ext_color = graph::colorPair(color[2 * y][x], color[2 * y + 1][x]);
  }
}
