#pragma once

#include "Input.h"
#include "Output.h"

#include <memory>

namespace ui {
  struct IOState;

  class IO {
  private:
    std::unique_ptr<IOState> state_;

  public:
    IO();
    ~IO();

    Input input;
    Output output;

    bool paused() const;
    bool quit() const;
  };
}
