#include "render.h"

#include "dim.h"
#include "graph.h"
#include "rts/Entity.h"
#include "ui/Sprite.h"

namespace ui {
  namespace {
    void draw(WINDOW* win, int y, int x, const Sprite& sprite) {
      mvwaddwstr(win, y, x, sprite.matrix(0, 0).c_str());
    }
  }
}

void ui::grid(WINDOW* win) {
  wattrset(win, graph::darkGrey());

  int y = 0;
  for (rts::Coordinate cellY = 0;;) {
    for (rts::Coordinate cellX = 1; cellX < Camera::width; ++cellX)
      mvwvline(win, y, 4 * cellX - 1, 0, dim::cellHeight);
    if (++cellY == Camera::height)
      break;
    ++y;
    mvwhline(win, y, 0, 0, dim::cellWidth);
    for (rts::Coordinate cellX = 0; cellX < Camera::width; ++cellX) {
      mvwaddch(win, y, 4 * cellX - 1, ACS_PLUS);
      whline(win, 0, dim::cellWidth);
    }
    ++y;
  }
}

void ui::render(WINDOW* win, const rts::World& world, const Camera& camera) {
  const auto& map = world.map;
  const auto& topLeft = camera.topLeft();
  const auto& bottomRight = camera.bottomRight();

  int y = 0;
  for (rts::Coordinate cellY = topLeft.y; cellY < bottomRight.y; ++cellY) {
    int x = 0;
    for (rts::Coordinate cellX = topLeft.x; cellX < bottomRight.x; ++cellX) {
      const auto& cell{map.at(cellX, cellY)};
      if (hasBlocker(cell)) {
        wattrset(win, graph::red());
        draw(win, y, x, getSprite(getBlocker(cell)));
      }
      else if (hasEntity(cell)) {
        wattrset(win, graph::green());
        draw(win, y, x, getSprite(*getEntity(cell)));
      }
      else {
        wattrset(win, graph::white());
        mvwaddstr(win, y, x, "   ");
      }
      x += dim::cellWidth + 1;
    }
    y += 2;
  }
}
