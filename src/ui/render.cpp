#include "render.h"

#include "graph.h"
#include "rts/Selection.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "ui/Sprite.h"
#include "ui/dim.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <set>
#include <vector>

ui::ScreenVector ui::toScreenVector(rts::Vector v) {
  return scale(v, dim::CellSizeEx);
}

ui::ScreenPoint ui::toScreenPoint(const Camera& camera, rts::Point p) {
  return toPoint(toScreenVector(p - camera.topLeft()));
}

ui::ScreenRect ui::toScreenRect(const Camera& camera, const rts::Rectangle& area) {
  return {toScreenPoint(camera, area.topLeft), toScreenVector(area.size) - ScreenVector{1, 1}};
}

void ui::grid(const Window& win) {
  graph::setColor(win, Color::DarkGray);

  int y = 0;
  for (rts::Coordinate cellY = 0;;) {
    for (rts::Coordinate cellX = 1; cellX < Camera::Size.x; ++cellX)
      mvwvline(win.w, y, cellX * dim::CellSizeEx.x - 1, 0, dim::CellSize.y);
    if (++cellY == Camera::Size.y)
      break;
    y += dim::CellSize.y;
    mvwhline(win.w, y, 0, 0, dim::CellSize.x);
    for (rts::Coordinate cellX = 1; cellX < Camera::Size.x; ++cellX) {
      mvwaddch(win.w, y, cellX * dim::CellSizeEx.x - 1, ACS_PLUS);
      whline(win.w, 0, dim::CellSize.x);
    }
    ++y;
  }
}

void ui::render(
    const Window& win,
    const rts::World& w,
    MapList maps,
    const Camera& camera,
    const rts::Selection& selection) {
  auto selectedItems{selection.items(w)};
  const std::set<rts::WorldObjectCPtr> selectedObjects{selectedItems.begin(), selectedItems.end()};
  const auto selectionColorPair{graph::colorPair(Color::Green)};

  std::vector<rts::WorldObjectCPtr> layers[rts::LayerCount];
  for (const rts::Map& m : maps) {
    for (auto* obj : w.objectsInArea(camera.area(), m)) {
      assert(obj->layer < rts::LayerCount);
      layers[obj->layer].push_back(obj);
    }
  }

  for (const auto& layer : layers) {
    for (auto* obj : layer) {
      render(win, w, camera, *obj);
      if (selectedObjects.find(obj) != selectedObjects.end())
        drawBoundingBox(win, camera, obj->area, selectionColorPair);
    }
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
    const rts::Rectangle& area) {
  const auto& spriteUi{getUpdatedSpriteUi(w, object)};
  assert(spriteUi.sprite().area(area.topLeft) == area);

  if (auto visibleArea{maybeIntersection(area, camera.area())}) {
    const ScreenRect drawRect{toScreenRect(camera, *visibleArea)};
    const ScreenVector topLeftInSprite{toScreenVector(visibleArea->topLeft - area.topLeft)};
    graph::drawFrame(
        win, spriteUi.frame(object.direction), drawRect, topLeftInSprite,
        spriteUi.defaultColor(object));
  }
}

void ui::highlight(const Window& win, const Camera& camera, rts::Point cell, short colorPair) {
  if (!camera.area().contains(cell))
    return;

  const rts::Vector cellInCamera{cell - camera.topLeft()};
  const ScreenVector topLeft{toScreenVector(cellInCamera)};
  util::geo::AtBorder atBorder{camera.area(), cell};
  graph::setColor(win, colorPair);
  if (!atBorder.top)
    mvwhline(win.w, topLeft.y - 1, topLeft.x, 0, dim::CellSize.x);
  if (!atBorder.bottom)
    mvwhline(win.w, topLeft.y + dim::CellSize.y, topLeft.x, 0, dim::CellSize.x);
  if (!atBorder.left)
    mvwvline(win.w, topLeft.y, topLeft.x - 1, 0, dim::CellSize.y);
  if (!atBorder.right)
    mvwvline(win.w, topLeft.y, topLeft.x + dim::CellSize.x, 0, dim::CellSize.y);
}

void ui::highlight(
    const Window& win, const Camera& camera, const rts::Rectangle& area, short colorPair) {
  graph::setColor(win, colorPair);
  graph::drawRectNoCorners(win, boundingBox(toScreenRect(camera, area)));
}

void ui::drawBoundingBox(
    const Window& win, const Camera& camera, rts::Point cell, short colorPair) {
  graph::setColor(win, colorPair);
  graph::drawRect(win, boundingBox(toScreenRect(camera, rts::Rectangle{cell, {1, 1}})));
}

void ui::drawBoundingBox(
    const Window& win, const Camera& camera, const rts::Rectangle& area, short colorPair) {
  graph::setColor(win, colorPair);
  graph::drawRect(win, boundingBox(toScreenRect(camera, area)));
}

#ifdef MAP_DEBUG
void ui::mapDebug(const Window& win, const rts::World& w, const Camera& camera) {
  const auto colorPair{graph::colorPair(Color::Blue)};
  for (rts::Point p : camera.area().points()) {
    if (w[p].debug.highlight)
      highlight(win, camera, p, colorPair);
  }
  graph::setColor(win, Color::DarkGray);
  for (rts::Point p : camera.area().points())
    printInCell(win, camera, p, std::to_string(w[p].segment).c_str());
}

void ui::printInCell(const Window& win, const Camera& camera, rts::Point p, const char* txt) {
  const ScreenVector topLeft{toScreenVector(p - camera.topLeft())};
  mvwprintw(win.w, topLeft.y, topLeft.x, txt);
}
#endif
