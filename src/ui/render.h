#pragma once

#include "rts/constants.h"
#include "rts/types.h"
#include "ui/Camera.h"
#include "ui/types.h"

#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

namespace ui {
  using MapList = std::initializer_list<std::reference_wrapper<const rts::Map>>;
  using LabelList = std::vector<std::pair<const Frame*, ScreenPoint>>;

  ScreenVector toScreenVector(rts::Vector v);
  ScreenPoint toScreenPoint(const Camera& camera, rts::Point p);
  ScreenRect toScreenRect(const Camera& camera, const rts::Rectangle& area);

  void grid(const Window& win);
  void render(
      const Window& win,
      const rts::World& w,
      MapList maps,
      const Camera& camera,
      const rts::Selection& selection);
  void render(
      const Window& win,
      const rts::World& w,
      const Camera& camera,
      const rts::WorldObject& object,
      bool selected,
      bool isTarget,
      LabelList& labels);
  void render(const Window& win, const LabelList& labels);
  void render(
      const Window& win,
      const rts::World& w,
      const Camera& camera,
      const rts::WorldObject& object,
      const rts::Rectangle& area);
  void highlight(const Window& win, const Camera& camera, rts::Point cell, short colorPair);
  void highlight(
      const Window& win, const Camera& camera, const rts::Rectangle& area, short colorPair);
  void drawBoundingBox(const Window& win, const Camera& camera, rts::Point cell, short colorPair);
  void drawBoundingBox(
      const Window& win, const Camera& camera, const rts::Rectangle& area, short colorPair);

#ifdef MAP_DEBUG
  void mapDebug(const Window& win, const rts::World& w, const Camera& camera);
  void printInCell(const Window& win, const Camera& camera, rts::Point p, const char* txt);
#endif
}
