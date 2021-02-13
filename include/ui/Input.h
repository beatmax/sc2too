#pragma once

#include "InputEvent.h"
#include "Player.h"
#include "rts/Command.h"
#include "rts/types.h"

#include <optional>

namespace ui {
  struct IOState;

  class Input {
  public:
    explicit Input(IOState& ioState);

    void init();
    void finish();
    std::optional<rts::SideCommand> process(
        rts::Engine& engine, const rts::World& w, Player& player);

  private:
    bool processInput(
        const rts::World& w, const Camera& camera, rts::Engine& engine, InputEvent& event);
    void transformMousePosition(
        const rts::World& w, const Camera& camera, InputMousePosition& position) const;
    void edgeScroll(InputEvent& event);

    IOState& ios_;
  };
}
