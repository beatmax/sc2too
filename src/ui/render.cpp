#include "render.h"

#include "SpriteMatrix.h"
#include "dim.h"
#include "rts/Entity.h"
#include "ui/Sprite.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <set>

namespace ui {
  namespace {
    using ScreenPoint = util::geo::Point;
    using ScreenVector = util::geo::Vector;
    using ScreenRect = util::geo::Rectangle;

    ScreenVector toScreenVector(rts::Vector v) {
      return {v.x * (dim::cellWidth + 1), v.y * (dim::cellHeight + 1)};
    };

    ScreenRect toScreenRect(const Camera& camera, const rts::Rectangle& area) {
      const rts::Vector relativeTopLeft{area.topLeft - camera.topLeft()};
      return {ScreenPoint{0, 0} + toScreenVector(relativeTopLeft),
              toScreenVector(area.size) - ScreenVector{1, 1}};
    }

    void draw(WINDOW* win, const Camera& camera, const rts::WorldObject& object) {
      // default for color pair 0 (e.g. entity's side color)
      wattrset(win, getDefaultColor(object));

      const Sprite& sprite{getSprite(object)};
      assert(sprite.area(object.area.topLeft) == object.area);

      const rts::Rectangle visibleArea{intersection(object.area, camera.area())};
      const ScreenRect drawRect{toScreenRect(camera, visibleArea)};
      const ScreenVector topLeftInSprite{toScreenVector(visibleArea.topLeft - object.area.topLeft)};

      const SpriteMatrix& matrix{sprite.matrix()};
      ScreenVector v{0, 0};
      for (; v.y < drawRect.size.y; ++v.y) {
        const auto sp{topLeftInSprite + v};
        mvwadd_wchnstr(
            win, drawRect.topLeft.y + v.y, drawRect.topLeft.x, &matrix(sp.y, sp.x),
            drawRect.size.x);
      }
    }
  }
}

void ui::grid(WINDOW* win) {
  wattrset(win, graph::darkGrey());

  int y = 0;
  for (rts::Coordinate cellY = 0;;) {
    for (rts::Coordinate cellX = 1; cellX < Camera::width; ++cellX)
      mvwvline(win, y, cellX * (dim::cellWidth + 1) - 1, 0, dim::cellHeight);
    if (++cellY == Camera::height)
      break;
    y += dim::cellHeight;
    mvwhline(win, y, 0, 0, dim::cellWidth);
    for (rts::Coordinate cellX = 1; cellX < Camera::width; ++cellX) {
      mvwaddch(win, y, cellX * (dim::cellWidth + 1) - 1, ACS_PLUS);
      whline(win, 0, dim::cellWidth);
    }
    ++y;
  }
}

void ui::highlight(WINDOW* win, const Camera& camera, rts::Point cell, int color) {
  if (!camera.area().contains(cell))
    return;

  const rts::Vector cellInCamera{cell - camera.topLeft()};
  const ScreenVector topLeft{toScreenVector(cellInCamera)};
  util::geo::AtBorder atBorder{camera.area(), cell};
  wattrset(win, color);
  if (!atBorder.top)
    mvwhline(win, topLeft.y - 1, topLeft.x, 0, dim::cellWidth);
  if (!atBorder.bottom)
    mvwhline(win, topLeft.y + dim::cellHeight, topLeft.x, 0, dim::cellWidth);
  if (!atBorder.left)
    mvwvline(win, topLeft.y, topLeft.x - 1, 0, dim::cellHeight);
  if (!atBorder.right)
    mvwvline(win, topLeft.y, topLeft.x + dim::cellWidth, 0, dim::cellHeight);
}

void ui::render(WINDOW* win, const rts::World& world, const Camera& camera) {
  const auto& topLeft = camera.topLeft();
  const auto& bottomRight = camera.bottomRight();

  std::set<rts::WorldObjectCPtr> visibleObjects;
  for (rts::Coordinate cellY = topLeft.y; cellY < bottomRight.y; ++cellY) {
    for (rts::Coordinate cellX = topLeft.x; cellX < bottomRight.x; ++cellX) {
      if (auto obj = world.objectPtrAt({cellX, cellY}))
        visibleObjects.insert(obj);
    }
  }

  for (rts::WorldObjectCPtr obj : visibleObjects)
    draw(win, camera, *obj);
}

#ifdef MAP_DEBUG
void ui::mapDebug(WINDOW* win, const rts::World& world, const Camera& camera) {
  const auto& topLeft = camera.topLeft();
  const auto& bottomRight = camera.bottomRight();

  for (rts::Coordinate cellY = topLeft.y; cellY < bottomRight.y; ++cellY) {
    for (rts::Coordinate cellX = topLeft.x; cellX < bottomRight.x; ++cellX) {
      if (auto color{world.map.at(cellX, cellY).debug.color}; color)
        highlight(win, camera, {cellX, cellY}, COLOR_PAIR(color));
    }
  }
}
#endif
