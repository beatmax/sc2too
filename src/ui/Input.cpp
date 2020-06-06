#include "ui/Input.h"

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

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
