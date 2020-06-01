#include "ui/Input.h"

#include <ncursesw/ncurses.h>

bool ui::Input::process(Player& player) {
  switch (getch()) {
    case KEY_RIGHT:
      player.camera.move(rts::Vector{1, 0});
      break;
    case KEY_LEFT:
      player.camera.move(rts::Vector{-1, 0});
      break;
    case KEY_UP:
      player.camera.move(rts::Vector{0, -1});
      break;
    case KEY_DOWN:
      player.camera.move(rts::Vector{0, 1});
      break;
    case 'q':
      return false;
  }
  return true;
}
