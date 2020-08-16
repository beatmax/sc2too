#pragma once

#include "InputEvent.h"
#include "Player.h"
#include "rts/Command.h"
#include "rts/types.h"

namespace ui {
  struct IOState;

  class Input {
  public:
    explicit Input(IOState& ioState);

    void init();
    void finish();
    void process(
        rts::Engine& engine, const rts::World& w, Player& player, rts::SideCommandList& commands);

  private:
    bool processKbInput(rts::Engine& engine, const InputEvent& event);
    bool processMouseInput(const InputEvent& event);
    InputEvent nextMouseEvent(const Camera& camera);
    InputEvent xMouseEvent();
    InputEvent edgeScrollEvent();

    IOState& ios_;
  };
}
