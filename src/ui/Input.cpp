#include "ui/Input.h"

#include <ncurses.h>

bool ui::Input::process() {
  switch (getch()) {
    case KEY_RIGHT:
      player_.camera.move(rts::Vector{1, 0});
      break;
    case KEY_LEFT:
      player_.camera.move(rts::Vector{-1, 0});
      break;
    case KEY_UP:
      player_.camera.move(rts::Vector{0, -1});
      break;
    case KEY_DOWN:
      player_.camera.move(rts::Vector{0, 1});
      break;
    case 'q':
      return false;
  }
  return true;
}
