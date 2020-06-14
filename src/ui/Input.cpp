#include "ui/Input.h"

#include "IOState.h"
#include "MenuImpl.h"
#include "X.h"
#include "dim.h"

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace {
  void initMouse() {
    mousemask(
        BUTTON1_PRESSED | BUTTON1_RELEASED | BUTTON2_PRESSED | BUTTON2_RELEASED | BUTTON3_PRESSED |
            BUTTON3_RELEASED | REPORT_MOUSE_POSITION,
        nullptr);
    mouseinterval(0);
  }
}

ui::Input::Input(IOState& ios) : ios_{ios} {
}

void ui::Input::init() {
  X::captureInput();

  keypad(stdscr, true);
  nodelay(stdscr, true);
  initMouse();
}

void ui::Input::process(Player& player) {
  if (ios_.menu.active()) {
    nodelay(stdscr, false);
    MenuImpl::processInput(ios_.menu, ios_.quit);
    if (!ios_.menu.active()) {
      X::init();
      X::captureInput();
      nodelay(stdscr, true);
    }
    return;
  }

  while (X::pendingEvent()) {
    InputEvent event{X::nextEvent()};
    if (processKbInput(event)) {
      if (ios_.menu.active()) {
        X::finish();
        return;
      }
    }
    else {
      player.processInput(event);
    }
  }

  int c;
  while ((c = getch()) != ERR) {
    if (c == KEY_MOUSE) {
      InputEvent event{nextMouseEvent(player.camera)};
      if (event.type != InputType::Unknown) {
        if (!processMouseInput(event))
          player.processInput(event);
      }
    }
  }

  player.camera.update();
}

bool ui::Input::processKbInput(const InputEvent& event) {
  if (event.type == InputType::KeyPress) {
    switch (event.symbol) {
      case InputKeySym::F10:
        ios_.menu.show();
        return true;
      default:
        break;
    }
  }
  return false;
}

bool ui::Input::processMouseInput(const InputEvent& event) {
  if (event.type == InputType::MousePress) {
    ios_.clickedCell = event.mouseCell;
    ios_.clickedButton = int(event.mouseButton);
    return true;
  }
  else if (event.type == InputType::MouseRelease) {
    ios_.clickedButton = 0;
    return true;
  }
  return false;
}

ui::InputEvent ui::Input::nextMouseEvent(const Camera& camera) {
  MEVENT event;
  InputEvent ievent;

  if (getmouse(&event) != OK || !wenclose(ios_.renderWin, event.y, event.x) ||
      !wmouse_trafo(ios_.renderWin, &event.y, &event.x, false)) {
    ievent.type = InputType::Unknown;
  }
  else {
    if (event.bstate & BUTTON1_PRESSED) {
      ievent.type = InputType::MousePress;
      ievent.mouseButton = InputButton::Button1;
    }
    else if (event.bstate & BUTTON2_PRESSED) {
      ievent.type = InputType::MousePress;
      ievent.mouseButton = InputButton::Button2;
    }
    else if (event.bstate & BUTTON3_PRESSED) {
      ievent.type = InputType::MousePress;
      ievent.mouseButton = InputButton::Button3;
    }
    else if (event.bstate & BUTTON1_RELEASED) {
      ievent.type = InputType::MouseRelease;
      ievent.mouseButton = InputButton::Button1;
    }
    else if (event.bstate & BUTTON2_RELEASED) {
      ievent.type = InputType::MouseRelease;
      ievent.mouseButton = InputButton::Button2;
    }
    else if (event.bstate & BUTTON3_RELEASED) {
      ievent.type = InputType::MouseRelease;
      ievent.mouseButton = InputButton::Button3;
    }
    else {
      ievent.type = InputType::Unknown;
    }

    ievent.mouseCell = camera.area().topLeft +
        rts::Vector{event.x / (dim::cellWidth + 1), event.y / (dim::cellHeight + 1)};
  }

  return ievent;
}
