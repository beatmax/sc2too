#pragma once

#include "ui/InputEvent.h"
#include "ui/types.h"

namespace ui::X {
  void init();
  void finish();
  PixelVector displaySize();
  void saveState();
  void restoreState();
  void grabInput();
  void releaseInput();
  bool pendingEvent();
  InputEvent nextEvent();
}
