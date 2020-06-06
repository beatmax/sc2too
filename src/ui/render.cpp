#include "render.h"

#include "rts/Entity.h"
#include "ui/Sprite.h"
#include "ui/dim.h"

#include <algorithm>
#include <cassert>

namespace ui {
  namespace {
    void draw(WINDOW* win, int y, int x, const Sprite& sprite, size_t spriteY, size_t spriteX) {
      int maxY, maxX;
      getmaxyx(win, maxY, maxX);

      for (const std::wstring& cellLine : sprite.matrix(spriteY, spriteX)) {
        if (y < maxY && !cellLine.empty()) {
          const int n = std::min(maxX - x, int(cellLine.size()));
          mvwaddnwstr(win, y++, x, cellLine.c_str(), n);
        }
      }
    }

    void draw(WINDOW* win, int y, int x, const rts::WorldObject& object, rts::Point p) {
      const Sprite& sprite{getSprite(object)};
      assert(sprite.area(object.area.topLeft) == object.area);
      assert(sprite.area(object.area.topLeft).contains(p));

      auto sp{p - object.area.topLeft};
      draw(win, y, x, sprite, sp.y, sp.x);
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
      rts::Point cellPos{cellX, cellY};
      const auto& cell{map.at(cellPos)};
      if (hasWorldObject(cell)) {
        wattrset(
            win,
            hasBlocker(cell) ? graph::red()
                             : hasResourceField(cell) ? graph::cyan() : graph::green());
        draw(win, y, x, getWorldObject(cell), cellPos);
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
