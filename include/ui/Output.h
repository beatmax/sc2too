#pragma once

#include "Player.h"
#include "rts/World.h"

namespace ui {
  struct IOState;

  class Output {
  public:
    explicit Output(IOState& ioState);
    ~Output();

    void init();
    void update(const rts::World& world, const Player& player);

  private:
    IOState& ios_;
  };
}
