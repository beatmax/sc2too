#pragma once

#include "Input.h"
#include "Output.h"

namespace ui {
  struct IO {
    Input input;
    Output output;

    IO();
  };
}
