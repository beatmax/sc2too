#pragma once

#include "ui/Frame.h"
#include "ui/Sprite.h"
#include "ui/types.h"

#include <string>

namespace ui::fx {
  Sprite flatten(const Sprite& s);
  Sprite unpower(const Sprite& s);
  void text(Frame& f, int x, int y, const std::wstring& txt, Color color);
}
