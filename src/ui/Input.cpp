#include "ui/Input.h"

#include "IOState.h"
#include "MenuImpl.h"
#include "X.h"
#include "dim.h"
#include "rts/Engine.h"

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
  std::optional<rts::Point> mouseCell;
  ui::ScrollDirection edgeScrollDirection{};

  void initMouse() {
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
  }

  void initMouseMotionReporting() { printf("\033[?1002h\n"); }

  void finishMouse() { printf("\033[?1002l\n"); }

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

void ui::Input::preInit() {
  // initialize mouse motion reporting before ncurses has been initialized
  // because the printf() call messes up the output (noticeable when the
  // terminal has just the minimum accepted size)
  initMouseMotionReporting();
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
  if (ios_.menu.active()) {
    nodelay(stdscr, false);
    MenuImpl::processInput(ios_.menu, ios_.quit);
    if (!ios_.menu.active()) {
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

  int c;
  while ((c = getch()) != ERR) {
    if (c == KEY_MOUSE) {
      InputEvent event{nextMouseEvent(player.camera)};
      if (event.type != InputType::Unknown) {
        if (!processMouseInput(event))
          if (auto sc{sideCommand(player.processInput(w, event))})
            return sc;
      }
    }
  }

  X::updatePointerState();

  if (InputEvent event{edgeScrollEvent()})
    if (auto sc{sideCommand(player.processInput(w, event))})
      return sc;

  while (X::pendingEvent()) {
    InputEvent event{X::nextEvent()};
    if (processKbInput(engine, event)) {
      if (ios_.menu.active()) {
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
  if (event.mouseCell)
    ios_.mousePosition = *event.mouseCell;

  return false;
}

ui::InputEvent ui::Input::nextMouseEvent(const Camera& camera) {
  MEVENT event;
  InputEvent ievent;

  if (getmouse(&event) != OK) {
    ievent.type = InputType::Unknown;
    return ievent;
  }

  if (wenclose(ios_.renderWin.w, event.y, event.x) &&
      wmouse_trafo(ios_.renderWin.w, &event.y, &event.x, false)) {
    mouseCell = camera.area().topLeft +
        rts::Vector{event.x / (dim::cellWidth + 1), event.y / (dim::cellHeight + 1)};
  }
  else {
    mouseCell.reset();
  }

  ievent.mouseCell = mouseCell;
  ievent.state = X::inputState;

  if (event.bstate & BUTTON1_PRESSED) {
    ievent.type = InputType::MousePress;
    ievent.mouseButton = InputButton::Button1;
    buttons |= Button1Pressed;
  }
  else if (event.bstate & BUTTON2_PRESSED) {
    ievent.type = InputType::MousePress;
    ievent.mouseButton = InputButton::Button2;
    buttons |= Button2Pressed;
  }
  else if (event.bstate & BUTTON3_PRESSED) {
    ievent.type = InputType::MousePress;
    ievent.mouseButton = InputButton::Button3;
    buttons |= Button3Pressed;
  }
  else if (event.bstate & BUTTON1_RELEASED) {
    ievent.type = InputType::MouseRelease;
    ievent.mouseButton = InputButton::Button1;
    buttons &= ~Button1Pressed;
  }
  else if (event.bstate & BUTTON2_RELEASED) {
    ievent.type = InputType::MouseRelease;
    ievent.mouseButton = InputButton::Button2;
    buttons &= ~Button2Pressed;
  }
  else if (event.bstate & BUTTON3_RELEASED) {
    ievent.type = InputType::MouseRelease;
    ievent.mouseButton = InputButton::Button3;
    buttons &= ~Button3Pressed;
  }
  else {
    ievent.type = InputType::MousePosition;
    ievent.mouseButton = InputButton::Unknown;
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
      (x == 0) ? ScrollDirectionLeft : (x == int(X::displayWidth - 1)) ? ScrollDirectionRight : 0};
  auto vDirection{
      (y == 0) ? ScrollDirectionUp : (y == int(X::displayHeight - 1)) ? ScrollDirectionDown : 0};
  ScrollDirection direction{hDirection | vDirection};

  if (edgeScrollDirection != direction) {
    edgeScrollDirection = direction;

    ievent.type = InputType::EdgeScroll;
    ievent.state = X::inputState;
    ievent.scrollDirection = direction;
  }

  return ievent;
}
