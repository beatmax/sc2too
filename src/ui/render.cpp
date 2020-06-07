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

  std::set<rts::WorldObjectCPtr> visibleObjects;
  for (rts::Coordinate cellY = topLeft.y; cellY < bottomRight.y; ++cellY) {
    for (rts::Coordinate cellX = topLeft.x; cellX < bottomRight.x; ++cellX) {
      const auto& cell{map.at(cellX, cellY)};
      if (hasWorldObject(cell))
        visibleObjects.insert(&getWorldObject(cell));
    }
  }

  for (rts::WorldObjectCPtr obj : visibleObjects)
    draw(win, camera, *obj);
}
