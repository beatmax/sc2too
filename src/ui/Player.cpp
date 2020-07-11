#include "ui/Player.h"

#include "rts/Entity.h"
#include "rts/World.h"

#include <map>

namespace ui {
  namespace {
    const std::map<InputKeySym, rts::Vector> cameraMoveMap{
        {InputKeySym::Right, rts::Vector{1, 0}},
        {InputKeySym::Left, rts::Vector{-1, 0}},
        {InputKeySym::Up, rts::Vector{0, -1}},
        {InputKeySym::Down, rts::Vector{0, 1}}};
  }
}

rts::WorldActionList ui::Player::processInput(const rts::World& world, const InputEvent& event) {
  if (event.type == InputType::KeyPress || event.type == InputType::KeyRelease) {
    if (auto it = cameraMoveMap.find(event.symbol); it != cameraMoveMap.end()) {
      if (event.type == InputType::KeyPress)
        camera.setMoveDirection(camera.moveDirection() + it->second);
      else
        camera.setMoveDirection(camera.moveDirection() - it->second);
    }
  }
  else if (event.type == InputType::MousePress && event.mouseButton == InputButton::Button3) {
    rts::WorldActionList actions;
    for (auto wid : selection) {
      if (auto entity = world.entities[wid]) {
        if (auto a{entity->abilities.begin()}; a != entity->abilities.end()) {
          rts::addActions(
              actions, entity->trigger(entity->abilities.id(*a), world, event.mouseCell));
        }
      }
    }
    return actions;
  }

  return {};
}
