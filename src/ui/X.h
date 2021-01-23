#pragma once

#include "ui/InputEvent.h"
#include "ui/types.h"

namespace ui::X {
  using PixelMatrix = Matrix<unsigned long>;

  void init();
  void finish();
  void saveState();
  void restoreState();
  void grabInput();
  void releaseInput();
  bool pendingEvent();
  InputEvent nextEvent();
  void updatePointerState();
  PixelMatrix getImage(const PixelRect& area);
  unsigned long getPixel(PixelPoint p);

  extern unsigned int displayWidth, displayHeight;
  extern int pointerX, pointerY;
  extern InputState inputState;
}
