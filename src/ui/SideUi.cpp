#include "ui/SideUi.h"

#include <cassert>

ui::SideUi::SideUi(int n) {
  switch (n) {
    case 0:
      color = Color::Cyan;
      break;
    case 1:
      color = Color::Red;
      break;
    default:
      assert(false);
      color = Color::White;
  }
}
