#pragma once

#include "Player.h"

namespace ui {

  class Input {
  public:
    explicit Input(Player& p) : player_{p} {}
    bool process();

  private:
    Player& player_;
  };
}
