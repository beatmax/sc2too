#pragma once

#include "Player.h"
#include "rts/World.h"

namespace ui {

  class Output {
  public:
    Output();
    ~Output();

    void update(const rts::World& world, const Player& player);
  };
}
