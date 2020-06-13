#include "ui/Input.h"

#include "MenuImpl.h"
#include "X.h"

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

void ui::Input::init(Menu& menu) {
  menu_ = &menu;
  X::captureInput();
}

bool ui::Input::process(Player& player) {
  if (menu_->active()) {
    bool quit;
    MenuImpl::processInput(*menu_, quit);
    if (!menu_->active()) {
      X::init();
      X::captureInput();
    }
    return !quit;
  }
  while (X::pendingEvent()) {
    InputEvent event{X::nextEvent()};
    if (processInput(event)) {
      if (menu_->active()) {
        X::finish();
        return true;
      }
    }
    else {
      player.processInput(event);
    }
  }
  player.camera.update();
  return true;
}

bool ui::Input::processInput(const InputEvent& event) {
  if (event.type == InputType::KeyPress) {
    switch (event.symbol) {
      case InputKeySym::F10:
        menu_->show();
        return true;
      default:
        break;
    }
  }
  return false;
}
