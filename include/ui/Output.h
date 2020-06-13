#pragma once

#include "Menu.h"
#include "Player.h"
#include "rts/World.h"

namespace ui {

  class Output {
  public:
    ~Output();

    void init(Menu& menu);
    void update(const rts::World& world, const Player& player);

  private:
    Menu* menu_;
  };
}
