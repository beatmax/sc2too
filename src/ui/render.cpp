#include "render.h"

#include "dim.h"
#include "graph.h"
#include "rts/Selection.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "ui/Sprite.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <set>

namespace ui {
  namespace {
    ScreenVector toScreenVector(rts::Vector v) {
      return {v.x * (dim::cellWidth + 1), v.y * (dim::cellHeight + 1)};
    };

    ScreenRect toScreenRect(const Camera& camera, const rts::Rectangle& area) {
      const rts::Vector relativeTopLeft{area.topLeft - camera.topLeft()};
      return {
          ScreenPoint{0, 0} + toScreenVector(relativeTopLeft),
          toScreenVector(area.size) - ScreenVector{1, 1}};
    }
  }
}

void ui::grid(const Window& win) {
  wattrset(win.w, graph::darkGrey());

  int y = 0;
  for (rts::Coordinate cellY = 0;;) {
    for (rts::Coordinate cellX = 1; cellX < Camera::width; ++cellX)
      mvwvline(win.w, y, cellX * (dim::cellWidth + 1) - 1, 0, dim::cellHeight);
    if (++cellY == Camera::height)
      break;
    y += dim::cellHeight;
    mvwhline(win.w, y, 0, 0, dim::cellWidth);
    for (rts::Coordinate cellX = 1; cellX < Camera::width; ++cellX) {
      mvwaddch(win.w, y, cellX * (dim::cellWidth + 1) - 1, ACS_PLUS);
      whline(win.w, 0, dim::cellWidth);
    }
    ++y;
  }
}

void ui::render(
    const Window& win,
    const rts::World& w,
    const rts::Map& m,
    const Camera& camera,
    const rts::Selection& selection) {
  auto selectedItems{selection.items(w)};
  const std::set<rts::WorldObjectCPtr> selectedObjects{selectedItems.begin(), selectedItems.end()};

  for (rts::WorldObjectCPtr obj : w.objectsInArea(camera.area(), m)) {
    render(win, w, camera, *obj);
    if (selectedObjects.find(obj) != selectedObjects.end())
      drawBoundingBox(win, camera, obj->area, graph::green());
  }
}

void ui::render(
    const Window& win, const rts::World& w, const Camera& camera, const rts::WorldObject& object) {
  const auto& spriteUi{getUpdatedSpriteUi(w, object)};

  assert(spriteUi.sprite().area(object.area.topLeft) == object.area);

  const rts::Rectangle visibleArea{intersection(object.area, camera.area())};
  const ScreenRect drawRect{toScreenRect(camera, visibleArea)};
  const ScreenVector topLeftInSprite{toScreenVector(visibleArea.topLeft - object.area.topLeft)};

  graph::drawFrame(
      win, spriteUi.frame(object.direction), drawRect, topLeftInSprite,
      spriteUi.defaultColor(object));
}

void ui::render(
    const Window& win,
    const rts::World& w,
    const Camera& camera,
    const rts::WorldObject& object,
    rts::Point center) {
  const auto& spriteUi{getUpdatedSpriteUi(w, object)};

  auto area{rectangleCenteredAt(center, object.area.size, w.map.area())};
  assert(spriteUi.sprite().area(area.topLeft) == area);

  if (auto visibleArea{maybeIntersection(area, camera.area())}) {
    const ScreenRect drawRect{toScreenRect(camera, *visibleArea)};
    const ScreenVector topLeftInSprite{toScreenVector(visibleArea->topLeft - area.topLeft)};
    graph::drawFrame(
        win, spriteUi.frame(object.direction), drawRect, topLeftInSprite,
        spriteUi.defaultColor(object));
  }
}

void ui::highlight(const Window& win, const Camera& camera, rts::Point cell, int color) {
  if (!camera.area().contains(cell))
    return;

  const rts::Vector cellInCamera{cell - camera.topLeft()};
  const ScreenVector topLeft{toScreenVector(cellInCamera)};
  util::geo::AtBorder atBorder{camera.area(), cell};
  wattrset(win.w, color);
  if (!atBorder.top)
    mvwhline(win.w, topLeft.y - 1, topLeft.x, 0, dim::cellWidth);
  if (!atBorder.bottom)
    mvwhline(win.w, topLeft.y + dim::cellHeight, topLeft.x, 0, dim::cellWidth);
  if (!atBorder.left)
    mvwvline(win.w, topLeft.y, topLeft.x - 1, 0, dim::cellHeight);
  if (!atBorder.right)
    mvwvline(win.w, topLeft.y, topLeft.x + dim::cellWidth, 0, dim::cellHeight);
}

void ui::highlight(const Window& win, const Camera& camera, const rts::Rectangle& area, int color) {
  wattrset(win.w, color);
  graph::drawRectNoCorners(win, boundingBox(toScreenRect(camera, area)));
}

void ui::drawBoundingBox(
    const Window& win, const Camera& camera, const rts::Rectangle& area, int color) {
  wattrset(win.w, color);
  graph::drawRect(win, boundingBox(toScreenRect(camera, area)));
}

#ifdef MAP_DEBUG
void ui::mapDebug(const Window& win, const rts::World& w, const Camera& camera) {
  for (rts::Point p : camera.area().points())
    if (w[p].debug.highlight)
      highlight(win, camera, p, graph::blue());
}
#endif
