#include "ui/Player.h"

#include "rts/Entity.h"
#include "rts/Side.h"
#include "rts/World.h"

namespace ui {
  namespace {
    int scrollKeyCnt{0};

    ScrollDirection keyScrollDirection(InputKeySym symbol) {
      switch (symbol) {
        case InputKeySym::Left:
          return ScrollDirectionLeft;
        case InputKeySym::Right:
          return ScrollDirectionRight;
        case InputKeySym::Up:
          return ScrollDirectionUp;
        case InputKeySym::Down:
          return ScrollDirectionDown;
        default:
          return 0;
      }
    }

    rts::Vector scrollDirectionVector(ScrollDirection sd) {
      return {(sd & ScrollDirectionLeft) ? -1 : (sd & ScrollDirectionRight) ? 1 : 0,
              (sd & ScrollDirectionUp) ? -1 : (sd & ScrollDirectionDown) ? 1 : 0};
    }

    void processKeyScrollEvent(InputType type, ScrollDirection sd, Camera& camera) {
      rts::Vector delta{scrollDirectionVector(sd)};
      if (type == InputType::KeyPress) {
        if (!scrollKeyCnt++)
          camera.setMoveDirection({0, 0});
      }
      else {
        --scrollKeyCnt;
        delta = -delta;
      }
      camera.setMoveDirection(camera.moveDirection() + delta);
    }
  }
}

std::optional<rts::Command> ui::Player::processInput(
    const rts::World& world, const InputEvent& event) {
  using SelectionCmd = rts::command::Selection;

  if (event.type == InputType::KeyPress || event.type == InputType::KeyRelease) {
    if (auto scrollDirection{keyScrollDirection(event.symbol)})
      processKeyScrollEvent(event.type, scrollDirection, camera);
  }
  else if (event.type == InputType::EdgeScroll && !scrollKeyCnt) {
    camera.setMoveDirection(scrollDirectionVector(event.scrollDirection));
  }
  else if (event.type == InputType::MousePress && event.mouseButton == InputButton::Button1) {
    auto& cell{world.map.at(event.mouseCell)};
    if (event.state & (ShiftPressed | ControlPressed | AltPressed)) {
      if (hasEntity(cell))
        return SelectionCmd{SelectionCmd::Add, {getEntityId(cell)}};
    }
    else {
      if (hasEntity(cell))
        return SelectionCmd{SelectionCmd::Set, {getEntityId(cell)}};
      else
        return SelectionCmd{SelectionCmd::Set, {}};
    }
  }
  else if (event.type == InputType::MousePress && event.mouseButton == InputButton::Button3) {
    return rts::command::TriggerDefaultAbility{event.mouseCell};
  }

  return std::nullopt;
}
