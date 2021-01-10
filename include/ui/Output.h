#pragma once

#include "Player.h"
#include "rts/types.h"

namespace ui {
  struct IOState;

  class Output {
  public:
    explicit Output(IOState& ioState);
    ~Output();

    void init();
    void update(const rts::Engine& engine, const rts::World& w, Player& player);

  private:
    void doUpdate(const rts::Engine& engine, const rts::World& w, const Player& player);

    IOState& ios_;
  };
}
