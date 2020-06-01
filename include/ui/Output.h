#pragma once

#include "Player.h"
#include "rts/World.h"

namespace ui {

  class Output {
  public:
    ~Output();

    void init();
    void update(const rts::World& world, const Player& player);
  };
}
