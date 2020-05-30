#include "sc2/ui.h"

namespace sc2::ui::sprite {
  // TODO can't use ncurses graph chars here (ncurses not yet initialized)
  ::ui::Sprite probe{"-o-\n"};
}

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::Entity&) const {
  return sprite::probe;
}
