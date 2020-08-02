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
  if (event.type == InputType::KeyPress || event.type == InputType::KeyRelease) {
    if (auto scrollDirection{keyScrollDirection(event.symbol)})
      processKeyScrollEvent(event.type, scrollDirection, camera);
  }
  else if (event.type == InputType::EdgeScroll && !scrollKeyCnt) {
    camera.setMoveDirection(scrollDirectionVector(event.scrollDirection));
  }
  else if (event.type == InputType::MousePress && event.mouseButton == InputButton::Button1) {
    using RC = rts::RelativeContent;
    using SelectionCmd = rts::command::Selection;

    auto rc{world.relativeContent(side, event.mouseCell)};
    if (rc == RC::Friend) {
      auto entity{getEntityId(world.map.at(event.mouseCell))};
      auto entities{(event.state & ControlPressed) ? visibleSameType(world, entity)
                                                   : rts::EntityIdList{entity}};
      if (event.state & ShiftPressed) {
        bool alreadySelected{world.sides[side].selection().contains(entity)};
        return SelectionCmd{alreadySelected ? SelectionCmd::Remove : SelectionCmd::Add, entities};
      }
      else {
        return SelectionCmd{SelectionCmd::Set, entities};
      }
    }
    else if (!(event.state & ShiftPressed)) {
      return SelectionCmd{SelectionCmd::Set, {}};
    }
  }
  else if (event.type == InputType::MousePress && event.mouseButton == InputButton::Button3) {
    return rts::command::TriggerDefaultAbility{event.mouseCell};
  }

  return std::nullopt;
}

rts::EntityIdList ui::Player::visibleSameType(const rts::World& world, rts::EntityId entity) {
  rts::EntityIdList result;
  const auto type{world.entities[entity].type};
  const auto& topLeft = camera.topLeft();
  const auto& bottomRight = camera.bottomRight();

  for (rts::Coordinate cellY = topLeft.y; cellY < bottomRight.y; ++cellY) {
    for (rts::Coordinate cellX = topLeft.x; cellX < bottomRight.x; ++cellX) {
      const auto& cell{world.map.at(cellX, cellY)};
      if (hasEntity(cell)) {
        auto eId{getEntityId(cell)};
        const auto& e{world.entities[eId]};
        if (e.type == type && e.side == side)
          result.push_back(eId);
      }
    }
  }

  return result;
}
