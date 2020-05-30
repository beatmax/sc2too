#include "render.h"

#include "dim.h"
#include "graph.h"

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
      chtype s[3]{' ', ' ', ' '};
      int attr;
      const auto& cell{map.at(cellX, cellY)};
      if (hasBlocker(cell)) {
        attr = graph::red();
        s[0] = s[1] = s[2] = ACS_CKBOARD;
      }
      else if (hasEntity(cell)) {
        attr = graph::green();
        s[1] = 'o';
      }
      else {
        attr = graph::white();
      }
      wattrset(win, attr);
      mvwaddch(win, y, x, s[0]);
      waddch(win, s[1]);
      waddch(win, s[2]);
      x += dim::cellWidth + 1;
    }
    y += 2;
  }
}
