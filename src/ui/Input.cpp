#include "ui/Input.h"

#include "IOState.h"
#include "MenuImpl.h"
#include "X.h"
#include "rts/Engine.h"
#include "ui/dim.h"

#include <cassert>
#include <optional>
#include <stdio.h>
#include <utility>

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace {
  ui::ScrollDirection edgeScrollDirection{};

  rts::Point getTargetPoint(const std::optional<rts::Command>& cmd) {
    if (cmd) {
      if (auto* c{std::get_if<rts::command::TriggerAbility>(&*cmd)})
        return c->target;
      if (auto* c{std::get_if<rts::command::TriggerDefaultAbility>(&*cmd)})
        return c->target;
    }
    return {-1, -1};
  }
}

ui::Input::Input(IOState& ios) : ios_{ios} {
}

void ui::Input::init() {
  do {
    X::releaseInput();
    ios_.xTermGeo.update();
    X::grabInput();
    // repeat if terminal was resized before grab
  } while (ios_.xTermGeo.outdated());

  // ncurses input, only used when menu is active
  raw();
  keypad(stdscr, true);
  nodelay(stdscr, false);
}

void ui::Input::finish() {
  X::releaseInput();
}

std::optional<rts::SideCommand> ui::Input::process(
    rts::Engine& engine, const rts::World& w, Player& player) {
  if (ios_.paused()) {
    MenuImpl::processInput(ios_.menu, ios_.quit);
    if (!ios_.paused()) {
      init();
      ios_.redraw = true;
    }
    return std::nullopt;
  }

  auto sideCommand = [&](std::optional<rts::Command>&& cmd) -> std::optional<rts::SideCommand> {
    ios_.targetPoint = getTargetPoint(cmd);
    if (cmd) {
      player.camera.update();
      return rts::SideCommand{player.side, std::move(*cmd)};
    }
    else {
      return std::nullopt;
    }
  };

  while (X::pendingEvent()) {
    InputEvent event{X::nextEvent()};
    if (processInput(w, player.camera, engine, event)) {
      if (ios_.paused()) {
        X::releaseInput();
        return std::nullopt;
      }
    }
    else if (auto sc{sideCommand(player.processInput(w, event))})
      return sc;
  }

  player.camera.update();
  return std::nullopt;
}

bool ui::Input::processInput(
    const rts::World& w, const Camera& camera, rts::Engine& engine, InputEvent& event) {
  switch (event.type) {
    case InputType::KeyPress:
      switch (event.symbol) {
        case InputKeySym::F10:
          ios_.menu.show();
          return true;
        case InputKeySym::F11:
          if (engine.gameSpeed() > 20)
            engine.gameSpeed(engine.gameSpeed() - 20);
          return true;
        case InputKeySym::F12:
          engine.gameSpeed(engine.gameSpeed() + 20);
          return true;
        default:
          break;
      }
      break;
    case InputType::MousePress:
    case InputType::MouseRelease:
    case InputType::MousePosition:
      transformMousePosition(w, camera, event.mousePosition);
      switch (event.mousePosition.area) {
        case InputMouseArea::None:
          edgeScroll(event);
          break;
        case InputMouseArea::Map:
          ios_.mouseMapPoint = event.mousePosition.point;
          break;
        default:
          break;
      }
    default:
      break;
  }
  return false;
}

void ui::Input::transformMousePosition(
    const rts::World& w, const Camera& camera, InputMousePosition& position) const {
  auto subp{ios_.xTermGeo.toSubcharPoint(position.point)};
  auto p{subp.point};
  if (wenclose(ios_.renderWin.w, p.y, p.x)) {
    if (wmouse_trafo(ios_.renderWin.w, &p.y, &p.x, false)) {
      position.area = InputMouseArea::Map;
      position.point = transformDiv(p, dim::CellSizeEx, {0, 0}, camera.area().topLeft);
    }
  }
  else if (wenclose(ios_.controlWin.w, p.y, p.x)) {
    if (wmouse_trafo(ios_.controlWin.w, &p.y, &p.x, false)) {
      if (dim::MinimapArea.contains(p)) {
        position.area = InputMouseArea::Minimap;
        position.point = clamp(
            toPoint(w.minimap.minimapToMap(
                scale((p - dim::MinimapArea.topLeft) + subp.subchar, FVector{1.f, 2.f}))),
            w.map.area());
      }
      else if (dim::SelectionAreaEx.contains(p)) {
        position.area = InputMouseArea::Selection;
        if (p.x == dim::SelectionAreaEx.topLeft.x)
          ++p.x;
        if (p.y == dim::SelectionAreaEx.topLeft.y)
          ++p.y;
        position.point = transformDiv(p, dim::CellSizeEx, dim::SelectionArea.topLeft, {0, 0});
      }
    }
  }
}

void ui::Input::edgeScroll(InputEvent& event) {
  if (event.state & ButtonMask)
    return;

  const auto& p{event.mousePosition.point};
  const auto& terminalSize{ios_.xTermGeo.rect().size};
  auto hDirection{
      (p.x <= 0)                             ? ScrollDirectionLeft
          : (p.x >= int(terminalSize.x - 1)) ? ScrollDirectionRight
                                             : 0};
  auto vDirection{
      (p.y <= 0)                             ? ScrollDirectionUp
          : (p.y >= int(terminalSize.y - 1)) ? ScrollDirectionDown
                                             : 0};
  ScrollDirection direction{hDirection | vDirection};

  if (edgeScrollDirection != direction) {
    edgeScrollDirection = direction;

    event.type = InputType::EdgeScroll;
    event.scrollDirection = direction;
  }
}
