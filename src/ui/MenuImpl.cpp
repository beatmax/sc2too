#include "MenuImpl.h"

void ui::MenuImpl::print(const Menu& menu, const Window& win) {
  mvwprintw(win.w, 0, 0, "G:return to game  Q:quit");
}

void ui::MenuImpl::processInput(Menu& menu, bool& quit) {
  quit = false;
  switch (getch()) {
    case 'g':
    case KEY_F(10):
      menu.hide();
      break;
    case 'q':
      quit = true;
      break;
  }
}
