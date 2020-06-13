#pragma once

#include "ui/InputEvent.h"

namespace ui::X {
  void init();
  void finish();
  void captureInput();
  bool pendingEvent();
  InputEvent nextEvent();
}
