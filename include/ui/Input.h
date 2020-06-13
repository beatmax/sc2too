#pragma once

#include "InputEvent.h"
#include "Menu.h"
#include "Player.h"

namespace ui {

  class Input {
  public:
    void init(Menu& menu);
    bool process(Player& player);

  private:
    bool processInput(const InputEvent& event);
    void processMenu(bool& quit);

    Menu* menu_;
  };
}
