#pragma once

#include "ui/InputEvent.h"

#include <optional>

namespace ui::X {
  void init();
  void finish();
  void captureInput();
  bool pendingEvent();
  InputEvent nextEvent();
  std::optional<InputEvent> pointerEvent();
}
