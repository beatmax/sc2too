#pragma once

#include "Input.h"
#include "Output.h"
#include "types.h"

#include <memory>

namespace ui {
  struct IOState;

  class IO {
  private:
    std::unique_ptr<IOState> state_;

  public:
    static void supervisor();

    IO();
    ~IO();

    Input input;
    Output output;

    void onGameStart(const rts::World& w);
    bool paused() const;
    bool quit() const;
    ScreenVector minimapSize() const;
  };
}
