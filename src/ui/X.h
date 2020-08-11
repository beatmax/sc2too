#pragma once

#include "ui/InputEvent.h"

namespace ui::X {
  void init();
  void finish();
  void grabInput();
  void releaseInput();
  bool pendingEvent();
  InputEvent nextEvent();
  void updatePointerState();

  extern unsigned int displayWidth, displayHeight;
  extern int pointerX, pointerY;
  extern InputState inputState;
}
