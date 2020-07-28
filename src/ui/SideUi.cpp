#include "ui/SideUi.h"

#include "graph.h"

#include <cassert>

ui::SideUi::SideUi(int n) {
  switch (n) {
    case 0:
      color = graph::cyan();
      break;
    case 1:
      color = graph::red();
      break;
    default:
      assert(false);
      color = graph::white();
  }
}
