#include "ui/Player.h"

#include <map>

namespace ui {
  namespace {
    const std::map<InputKeySym, rts::Vector> cameraMoveMap{{InputKeySym::Right, rts::Vector{1, 0}},
                                                           {InputKeySym::Left, rts::Vector{-1, 0}},
                                                           {InputKeySym::Up, rts::Vector{0, -1}},
                                                           {InputKeySym::Down, rts::Vector{0, 1}}};
  }
}

void ui::Player::processInput(const InputEvent& event) {
  if (event.type == InputType::KeyPress || event.type == InputType::KeyRelease) {
    if (auto it = cameraMoveMap.find(event.symbol); it != cameraMoveMap.end()) {
      if (event.type == InputType::KeyPress)
        camera.setMoveDirection(camera.moveDirection() + it->second);
      else
        camera.setMoveDirection(camera.moveDirection() - it->second);
    }
  }
}
