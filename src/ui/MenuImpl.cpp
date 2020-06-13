#include "MenuImpl.h"

void ui::MenuImpl::print(const Menu& menu, WINDOW* win) {
  mvwprintw(win, 0, 0, "F10:continue Q:quit");
}

void ui::MenuImpl::processInput(Menu& menu, bool& quit) {
  quit = false;
  switch (getch()) {
    case KEY_F(10):
      menu.hide();
      break;
    case 'q':
      quit = true;
      break;
  }
}
