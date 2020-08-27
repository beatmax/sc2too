#include "ui/Player.h"

#include "Layout.h"
#include "rts/Side.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "util/geo.h"

#include <cassert>
#include <utility>

namespace ui {
  namespace {
    int scrollKeyCnt{0};

    rts::Vector scrollDirectionVector(ScrollDirection sd) {
      return {
          (sd & ScrollDirectionLeft) ? -1 : (sd & ScrollDirectionRight) ? 1 : 0,
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
  auto cmd{doProcessInput(w, event)};
  if (cmd)
    selectingAbilityTarget.reset();
  return cmd;
}

std::optional<rts::Command> ui::Player::doProcessInput(
    const rts::World& w, const InputEvent& event) {
  using ControlGroupCmd = rts::command::ControlGroup;
  using SelectionCmd = rts::command::Selection;
  using SelectionSubgroupCmd = rts::command::SelectionSubgroup;
  using RC = rts::RelativeContent;

  switch (event.type) {
    case InputType::Unknown:
      break;
    case InputType::KeyPress:
#ifndef NDEBUG
      if (event.symbol == InputKeySym::X && (event.state & ControlPressed)) {
        using DebugCmd = rts::command::Debug;
        return DebugCmd{DebugCmd::Destroy};
      }
#endif
      if (event.symbol == InputKeySym::Tab) {
        return SelectionSubgroupCmd{
            (event.state & ShiftPressed) ? SelectionSubgroupCmd::Previous
                                         : SelectionSubgroupCmd::Next};
      }
      else if (int digit{getDigit(event.symbol)}; digit >= 0) {
        return ControlGroupCmd{
            (event.state & ShiftPressed)
                ? ControlGroupCmd::Add
                : (event.state & (ControlPressed | AltPressed)) ? ControlGroupCmd::Set
                                                                : ControlGroupCmd::Select,
            bool(event.state & AltPressed), rts::ControlGroupId(digit)};
      }
      else if ((selectionBox || selectingAbilityTarget) && Layout::cancel(event.symbol)) {
        selectionBox.reset();
        selectingAbilityTarget.reset();
      }
      else if (auto abilityIndex{Layout::abilityIndex(event.symbol)};
               abilityIndex != rts::AbilityInstanceIndex::None) {
        assert(abilityIndex < rts::MaxUnitAbilities);
        auto subgroupType{w[side].selection().subgroupType(w)};
        if (subgroupType) {
          const auto& type{w[subgroupType]};
          if (auto a{type.abilities[abilityIndex].abilityId}) {
            if (w[a].targetType == rts::Ability::TargetType::None) {
              return rts::command::TriggerAbility{a, {-1, -1}};
            }
            else {
              selectingAbilityTarget = a;
              selectionBox.reset();
            }
          }
        }
        break;
      }
      // pass-through
    case InputType::KeyRelease:
      if (auto scrollDirection{Layout::scrollDirection(event.symbol)})
        processKeyScrollEvent(event.type, scrollDirection, camera);
      break;
    case InputType::MousePress:
      if (event.mouseCell) {
        auto mouseCell{*event.mouseCell};

        if (event.mouseButton == InputButton::Button1) {
          if (selectingAbilityTarget)
            return rts::command::TriggerAbility{*selectingAbilityTarget, mouseCell};
          auto rc{w.relativeContent(side, mouseCell)};
          if (rc == RC::Friend) {
            auto unit{w.unitId(mouseCell)};
            auto units{
                (event.state & ControlPressed) ? visibleSameType(w, unit) : rts::UnitIdList{unit}};
            if (event.state & ShiftPressed) {
              bool alreadySelected{w[side].selection().contains(unit)};
              return SelectionCmd{
                  alreadySelected ? SelectionCmd::Remove : SelectionCmd::Add, units};
            }
            else {
              return SelectionCmd{SelectionCmd::Set, std::move(units)};
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
        auto units{w.unitsInArea(*selectionBox, side)};
        selectionBox.reset();
        return SelectionCmd{
            (event.state & ShiftPressed) ? SelectionCmd::Add : SelectionCmd::Set, std::move(units)};
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

rts::UnitIdList ui::Player::visibleSameType(const rts::World& w, rts::UnitId unit) {
  const auto type{w[unit].type};
  return w.unitsInArea(camera.area(), side, type);
}
