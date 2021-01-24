#include "ui/Input.h"

#include "IOState.h"
#include "MenuImpl.h"
#include "X.h"
#include "rts/Engine.h"
#include "ui/dim.h"

#include <cassert>
#include <optional>
#include <stdio.h>

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace {
  ui::InputState buttons{};
  ui::InputMousePosition mousePosition{ui::InputMouseArea::None, {-1, -1}};
  bool mousePositionInitialized{false};
  bool mousePositionUpdated{false};
  ui::ScrollDirection edgeScrollDirection{};
  bool motionReportingEnabled{false};

  void initMouse() {
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
  }

  void initMouseMotionReporting() {
    printf("\033[?1003h\n");
    motionReportingEnabled = true;
  }

  void finishMouseMotionReporting() {
    if (motionReportingEnabled) {
      printf("\033[?1003l\n");
      motionReportingEnabled = false;
    }
  }

  void finishMouse() { finishMouseMotionReporting(); }

  std::optional<ui::ScreenPoint> absoluteMousePosition() {
    std::optional<ui::ScreenPoint> p;
    int c;
    while ((c = getch()) != ERR) {
      if (c == KEY_MOUSE) {
        MEVENT event;
        if (getmouse(&event) == OK)
          p = {event.x, event.y};
      }
    }
    return p;
  }

  rts::Point getTarget(const std::optional<rts::Command>& cmd) {
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
  X::grabInput();

  raw();
  keypad(stdscr, true);
  nodelay(stdscr, true);
  initMouse();
}

void ui::Input::finish() {
  finishMouse();
  X::releaseInput();
}

std::optional<rts::SideCommand> ui::Input::process(
    rts::Engine& engine, const rts::World& w, Player& player) {
  if (ios_.paused()) {
    nodelay(stdscr, false);
    MenuImpl::processInput(ios_.menu, ios_.quit);
    if (!ios_.paused()) {
      X::init();
      X::grabInput();
      nodelay(stdscr, true);
    }
    return std::nullopt;
  }

  auto sideCommand = [&](std::optional<rts::Command>&& cmd) -> std::optional<rts::SideCommand> {
    ios_.clickedTarget = getTarget(cmd);
    if (cmd) {
      player.camera.update();
      return rts::SideCommand{player.side, std::move(*cmd)};
    }
    else {
      return std::nullopt;
    }
  };

  X::updatePointerState();

  mousePositionUpdated = false;
  if (auto ap{absoluteMousePosition()}) {
    ios_.pixelTr.update({*ap, {X::pointerX, X::pointerY}});
    updateMousePosition(w, *ap, player.camera);
  }

  if (mousePositionInitialized) {
    while (InputEvent event{nextMouseEvent()}) {
      if (!processMouseInput(event)) {
        if (auto sc{sideCommand(player.processInput(w, event))})
          return sc;
      }
    }
  }

  if (InputEvent event{edgeScrollEvent()})
    if (auto sc{sideCommand(player.processInput(w, event))})
      return sc;

  while (X::pendingEvent()) {
    InputEvent event{X::nextEvent()};
    if (processKbInput(engine, event)) {
      if (ios_.paused()) {
        X::finish();
        return std::nullopt;
      }
    }
    else {
      if (auto sc{sideCommand(player.processInput(w, event))})
        return sc;
    }
  }

  player.camera.update();
  return std::nullopt;
}

bool ui::Input::processKbInput(rts::Engine& engine, const InputEvent& event) {
  if (event.type == InputType::KeyPress) {
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
  }
  return false;
}

bool ui::Input::processMouseInput(const InputEvent& event) {
  ios_.mouseButtons = buttons >> 8;
  if (event.type != InputType::MousePosition)
    ++ios_.clicks;
  if (event.mousePosition.area == InputMouseArea::Map)
    ios_.mousePosition = event.mousePosition.point;

  if (!motionReportingEnabled && event.type == InputType::MousePress) {
    // on some terminals ncurses gets stuck if motion reporting is enabled before a click;
    // do not disable it on MouseRelease (has strange effects on some other terminals)
    initMouseMotionReporting();
  }

  return false;
}

void ui::Input::updateMousePosition(const rts::World& w, ScreenPoint p, const Camera& camera) {
  mousePosition.area = InputMouseArea::None;
  mousePositionInitialized = true;
  mousePositionUpdated = true;

  if (wenclose(ios_.renderWin.w, p.y, p.x)) {
    if (wmouse_trafo(ios_.renderWin.w, &p.y, &p.x, false)) {
      mousePosition.area = InputMouseArea::Map;
      mousePosition.point = transformDiv(p, dim::CellSizeEx, {0, 0}, camera.area().topLeft);
    }
  }
  else if (wenclose(ios_.controlWin.w, p.y, p.x)) {
    if (wmouse_trafo(ios_.controlWin.w, &p.y, &p.x, false)) {
      if (dim::MinimapArea.contains(p)) {
        auto subp{relativeSubcharPoint(ios_.controlWin, p, dim::MinimapArea)};
        mousePosition.area = InputMouseArea::Minimap;
        mousePosition.point = clamp(
            toPoint(w.minimap.minimapToMap(scale(subp.point + subp.subchar, FVector{1.f, 2.f}))),
            w.map.area());
      }
      else if (dim::SelectionAreaEx.contains(p)) {
        mousePosition.area = InputMouseArea::Selection;
        if (p.x == dim::SelectionAreaEx.topLeft.x)
          ++p.x;
        if (p.y == dim::SelectionAreaEx.topLeft.y)
          ++p.y;
        mousePosition.point = transformDiv(p, dim::CellSizeEx, dim::SelectionArea.topLeft, {0, 0});
      }
    }
  }
}

ui::SubcharPoint ui::Input::relativeSubcharPoint(
    const Window& win, ScreenPoint p, const ScreenRect& area) const {
  if (auto subp{ios_.pixelTr.toSubcharPoint({X::pointerX, X::pointerY})}) {
    auto q{toPoint(subp->point - ScreenVector{win.beginX, win.beginY} - area.topLeft)};
    if (area.contains(q))
      return {q, subp->subchar};
  }
  return {toPoint(p - area.topLeft), {0.5f, 0.5f}};
}

ui::InputEvent ui::Input::nextMouseEvent() {
  InputEvent ievent;
  ievent.mousePosition = mousePosition;
  ievent.state = X::inputState;

  // use button state from X to generate mouse events; ncurses misses events
  // sometimes, and has many issues when motion reporting is enabled

  auto diff{buttons ^ (X::inputState & ButtonMask)};

  if (diff) {
    if (diff & Button1Pressed) {
      diff = Button1Pressed;
      ievent.mouseButton = InputButton::Button1;
    }
    else if (diff & Button2Pressed) {
      diff = Button2Pressed;
      ievent.mouseButton = InputButton::Button2;
    }
    else {
      assert(diff & Button3Pressed);
      diff = Button3Pressed;
      ievent.mouseButton = InputButton::Button3;
    }

    buttons ^= diff;
    ievent.type = (X::inputState & diff) ? InputType::MousePress : InputType::MouseRelease;
  }
  else if (mousePositionUpdated) {
    mousePositionUpdated = false;
    ievent.type = InputType::MousePosition;
    ievent.mouseButton = InputButton::Unknown;
  }
  else {
    ievent.type = InputType::Unknown;
  }

  return ievent;
}

ui::InputEvent ui::Input::edgeScrollEvent() {
  InputEvent ievent;
  ievent.type = InputType::Unknown;

  if (buttons)
    return ievent;

  auto x = X::pointerX;
  auto y = X::pointerY;
  auto hDirection{
      (x == 0)                              ? ScrollDirectionLeft
          : (x == int(X::displayWidth - 1)) ? ScrollDirectionRight
                                            : 0};
  auto vDirection{
      (y == 0)                               ? ScrollDirectionUp
          : (y == int(X::displayHeight - 1)) ? ScrollDirectionDown
                                             : 0};
  ScrollDirection direction{hDirection | vDirection};

  if (edgeScrollDirection != direction) {
    edgeScrollDirection = direction;

    ievent.type = InputType::EdgeScroll;
    ievent.state = X::inputState;
    ievent.scrollDirection = direction;
  }

  return ievent;
}
