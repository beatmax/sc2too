#include "ui/Player.h"

#include "rts/Entity.h"
#include "rts/Side.h"
#include "rts/World.h"
#include "util/geo.h"

#include <utility>

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

std::optional<rts::Command> ui::Player::processInput(const rts::World& w, const InputEvent& event) {
  using ControlGroupCmd = rts::command::ControlGroup;
  using SelectionCmd = rts::command::Selection;
  using RC = rts::RelativeContent;

  switch (event.type) {
    case InputType::Unknown:
      break;
    case InputType::KeyPress:
      if (int digit{getDigit(event.symbol)}; digit >= 0) {
        return ControlGroupCmd{(event.state & ShiftPressed)
                                   ? ControlGroupCmd::Add
                                   : (event.state & (ControlPressed | AltPressed))
                                       ? ControlGroupCmd::Set
                                       : ControlGroupCmd::Select,
                               bool(event.state & AltPressed), rts::ControlGroupId(digit)};
      }
      // pass-through
    case InputType::KeyRelease:
      if (auto scrollDirection{keyScrollDirection(event.symbol)})
        processKeyScrollEvent(event.type, scrollDirection, camera);
      break;
    case InputType::MousePress:
      if (event.mouseCell) {
        auto mouseCell{*event.mouseCell};

        if (event.mouseButton == InputButton::Button1) {
          auto rc{w.relativeContent(side, mouseCell)};
          if (rc == RC::Friend) {
            auto entity{w.entityId(mouseCell)};
            auto entities{(event.state & ControlPressed) ? visibleSameType(w, entity)
                                                         : rts::EntityIdList{entity}};
            if (event.state & ShiftPressed) {
              bool alreadySelected{w[side].selection().contains(entity)};
              return SelectionCmd{alreadySelected ? SelectionCmd::Remove : SelectionCmd::Add,
                                  entities};
            }
            else {
              return SelectionCmd{SelectionCmd::Set, std::move(entities)};
            }
          }
          else if (rc == RC::Ground) {
            selectionBox = {mouseCell, {1, 1}};
            selectionBoxStart_ = mouseCell;
          }
          else if (!(event.state & ShiftPressed)) {
            return SelectionCmd{SelectionCmd::Set, {}};
          }
        }
        else if (event.mouseButton == InputButton::Button3) {
          return rts::command::TriggerDefaultAbility{mouseCell};
        }
      }
      break;
    case InputType::MouseRelease:
      if (selectionBox && event.mouseButton == InputButton::Button1) {
        auto entities{w.entitiesInArea(*selectionBox, side)};
        selectionBox.reset();
        return SelectionCmd{(event.state & ShiftPressed) ? SelectionCmd::Add : SelectionCmd::Set,
                            std::move(entities)};
      }
      break;
    case InputType::MousePosition:
      if (selectionBox && event.mouseCell) {
        selectionBox = util::geo::fixNegativeSize(
            {selectionBoxStart_, *event.mouseCell - selectionBoxStart_ + rts::Vector{1, 1}});
      }
      break;
    case InputType::EdgeScroll:
      if (!scrollKeyCnt)
        camera.setMoveDirection(scrollDirectionVector(event.scrollDirection));
      break;
  }

  return std::nullopt;
}

rts::EntityIdList ui::Player::visibleSameType(const rts::World& w, rts::EntityId entity) {
  const auto type{w[entity].type};
  return w.entitiesInArea(camera.area(), side, type);
}
