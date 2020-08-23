#pragma once

#include "rts/types.h"
#include "ui/InputEvent.h"
#include "ui/types.h"

namespace ui {
  class Layout {
  public:
    static bool cancel(InputKeySym symbol);
    static ScrollDirection scrollDirection(InputKeySym symbol);
    static rts::AbilityInstanceIndex abilityIndex(InputKeySym symbol);
    static char abilityKey(rts::AbilityInstanceIndex ai);
  };
}
