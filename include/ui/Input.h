#pragma once

#include "InputEvent.h"
#include "Player.h"

namespace ui {
  struct IOState;

  class Input {
  public:
    explicit Input(IOState& ioState);

    void init();
    void process(Player& player);

  private:
    bool processKbInput(const InputEvent& event);
    bool processMouseInput(const InputEvent& event);
    InputEvent nextMouseEvent(const Camera& camera);

    IOState& ios_;
  };
}
