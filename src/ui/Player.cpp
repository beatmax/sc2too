#include "ui/Player.h"

#include "Layout.h"
#include "rts/Side.h"
#include "rts/Unit.h"
#include "rts/World.h"
#include "rts/abilities.h"
#include "util/ScopeExit.h"
#include "util/algorithm.h"
#include "util/geo.h"

#include <cassert>
#include <utility>

namespace ui {
  namespace {
    int scrollKeyCnt{0};

    rts::Vector scrollDirectionVector(ScrollDirection sd) {
      return {
          (sd & ScrollDirectionLeft)        ? -1
              : (sd & ScrollDirectionRight) ? 1
                                            : 0,
          (sd & ScrollDirectionUp)         ? -1
              : (sd & ScrollDirectionDown) ? 1
                                           : 0};
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

void ui::Player::update(const rts::World& w) {
  worldView.update(w, w[side]);

  if (state_ != State::Default) {
    if (state_ == State::BuildingPrototype && w[side].prototype()) {
      selectingAbilityTarget = {lastBuildAbilityIndex_};
    }
    else if (state_ == State::BuildTriggered && !w[side].prototype()) {
      abilityPage = 0;
      selectingAbilityTarget.reset();
    }
    state_ = State::Default;
  }
}

std::optional<rts::Command> ui::Player::processInput(const rts::World& w, const InputEvent& event) {
  using ControlGroupCmd = rts::command::ControlGroup;
  using SelectionCmd = rts::command::Selection;
  using SelectionSubgroupCmd = rts::command::SelectionSubgroup;
  using RC = rts::RelativeContent;

  Event previousEvent{std::exchange(lastEvent_, {event, Clock::now()})};

  util::ScopeExit finishTargetSelection;
  if (selectingAbilityTarget)
    finishTargetSelection = [this]() { selectingAbilityTarget.reset(); };

  util::ScopeExit goToMainAbilityPage;
  if (abilityPage)
    goToMainAbilityPage = [this]() { abilityPage = 0; };

  const auto& mousePoint{event.mousePosition.point};

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
      if (event.symbol == InputKeySym::Space) {
        cycleBase(w);
      }
      else if (event.symbol == InputKeySym::Tab) {
        return SelectionSubgroupCmd{
            (event.state & ShiftPressed) ? SelectionSubgroupCmd::Previous
                                         : SelectionSubgroupCmd::Next};
      }
      else if (int digit{getDigit(event.symbol)}; digit >= 0) {
        if (event.state == 0) {
          if (previousEvent.e.type == InputType::KeyRelease &&
              previousEvent.e.symbol == event.symbol && previousEvent.e.state == 0 &&
              (lastEvent_.time - previousEvent.time) < std::chrono::seconds{1}) {
            if (const auto& items{w[side].selection().items(w)}; !items.empty()) {
              camera.setCenter(rts::World::centralUnit(items).area.center());
              break;
            }
          }
        }
        return ControlGroupCmd{
            (event.state & ShiftPressed)                        ? ControlGroupCmd::Add
                : (event.state & (ControlPressed | AltPressed)) ? ControlGroupCmd::Set
                                                                : ControlGroupCmd::Select,
            bool(event.state & AltPressed), rts::ControlGroupId(digit)};
      }
      else if (int fkey{getFunctionKeyNumber(event.symbol)};
               fkey > 0 && fkey <= int(MaxCameraPositions)) {
        --fkey;
        if (event.state & (ControlPressed | AltPressed | ShiftPressed))
          cameraPositions_[fkey] = camera.topLeft();
        else if (cameraPositions_[fkey])
          camera.setTopLeft(*cameraPositions_[fkey]);
      }
      else if (Layout::cancel(event.symbol)) {
        abilityPage = 0;
        selectionBox.reset();
        selectingAbilityTarget.reset();
        if (w[side].prototype())
          return rts::command::Cancel{};
      }
      else if (auto abilityIndex{Layout::abilityIndex(event.symbol, abilityPage)};
               abilityIndex != rts::AbilityInstanceIndex::None) {
        assert(abilityIndex < rts::MaxUnitAbilities);
        const auto& subgroup{w[side].selection().subgroup(w)};
        if (subgroup.abilityEnabled(w, abilityIndex)) {
          const auto& ai{*subgroup.abilities[abilityIndex]};
          if (ai.kind != rts::abilities::Kind::None) {
            if (ai.targetType == rts::abilities::TargetType::None) {
              const bool enqueue(event.state & ShiftPressed);
              return rts::command::TriggerAbility{abilityIndex, {-1, -1}, enqueue};
            }
            selectionBox.reset();
            if (ai.kind != rts::abilities::Kind::Build) {
              selectingAbilityTarget = {abilityIndex};
            }
            else if (subgroup.type) {
              goToMainAbilityPage.reset();
              state_ = State::BuildingPrototype;
              lastBuildAbilityIndex_ = abilityIndex;
              return rts::command::BuildPrototype{ai.desc<rts::abilities::Build>().type};
            }
          }
          else if (ai.goToPage) {
            abilityPage = ai.goToPage;
            selectingAbilityTarget.reset();
          }
        }
      }
      else if (auto scrollDirection{Layout::scrollDirection(event.symbol)})
        processKeyScrollEvent(event.type, scrollDirection, camera);
      break;
    case InputType::KeyRelease:
      if (auto scrollDirection{Layout::scrollDirection(event.symbol)})
        processKeyScrollEvent(event.type, scrollDirection, camera);
      else if (event.symbol == InputKeySym::Shift_L || event.symbol == InputKeySym::Shift_R) {
        if (selectingAbilityTarget && selectingAbilityTarget->afterEnqueue) {
          abilityPage = 0;
          selectingAbilityTarget.reset();
          return rts::command::Cancel{};
        }
      }
      break;
    case InputType::MousePress:
      switch (event.mousePosition.area) {
        case InputMouseArea::Map:
          if (event.mouseButton == InputButton::Button1) {
            if (selectingAbilityTarget) {
              if (w[side].prototype()) {
                finishTargetSelection.reset();
                goToMainAbilityPage.reset();
                state_ = State::BuildTriggered;
              }
              const bool enqueue(event.state & ShiftPressed);
              selectingAbilityTarget->afterEnqueue = enqueue;
              return rts::command::TriggerAbility{
                  selectingAbilityTarget->abilityIndex, mousePoint, enqueue};
            }
            auto rc{w.relativeContent(side, mousePoint)};
            if (rc == RC::Friend || rc == RC::FriendResource) {
              auto unit{w.unitId(mousePoint)};
              auto units{
                  (event.state & ControlPressed) ? visibleSameType(w, unit)
                                                 : rts::UnitIdList{unit}};
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
              selectionBox = {mousePoint, {1, 1}};
              selectionBoxStart_ = mousePoint;
            }
            else if (!(event.state & ShiftPressed)) {
              return SelectionCmd{SelectionCmd::Set, {}};
            }
          }
          else if (event.mouseButton == InputButton::Button3) {
            const bool enqueue(event.state & ShiftPressed);
            return rts::command::TriggerDefaultAbility{mousePoint, enqueue};
          }
          break;
        case InputMouseArea::Minimap:
          if (event.mouseButton == InputButton::Button1) {
            if (selectingAbilityTarget && !w[side].prototype()) {
              const bool enqueue(event.state & ShiftPressed);
              selectingAbilityTarget->afterEnqueue = enqueue;
              return rts::command::TriggerAbility{
                  selectingAbilityTarget->abilityIndex, mousePoint, enqueue};
            }
            camera.setCenter(mousePoint);
            minimapDrag = true;
          }
          else if (event.mouseButton == InputButton::Button3) {
            const bool enqueue(event.state & ShiftPressed);
            return rts::command::TriggerDefaultAbility{mousePoint, enqueue, true};
          }
          break;
        case InputMouseArea::Selection:
          if (event.mouseButton == InputButton::Button1) {
            if (auto unit{worldView.selection[mousePoint]}) {
              auto units{
                  (event.state & ControlPressed) ? selectedSameType(w, unit)
                                                 : rts::UnitIdList{unit}};
              return SelectionCmd{
                  (event.state & ShiftPressed) ? SelectionCmd::Remove : SelectionCmd::Set, units};
            }
          }
          break;
        default:
          break;
      }
      break;
    case InputType::MouseRelease:
      if (selectionBox && event.mouseButton == InputButton::Button1) {
        auto units{w.unitsInAreaForSelection(*selectionBox, side)};

        selectionBox.reset();
        return SelectionCmd{
            (event.state & ShiftPressed) ? SelectionCmd::Add : SelectionCmd::Set, std::move(units)};
      }
      minimapDrag = false;
      break;
    case InputType::MousePosition:
      switch (event.mousePosition.area) {
        case InputMouseArea::Map:
          if (selectionBox) {
            selectionBox = util::geo::fixNegativeSize(
                {selectionBoxStart_, mousePoint - selectionBoxStart_ + rts::Vector{1, 1}});
          }
          break;
        case InputMouseArea::Minimap:
          if (minimapDrag)
            camera.setCenter(mousePoint);
          break;
        default:
          break;
      }
      break;
    case InputType::EdgeScroll:
      if (!scrollKeyCnt)
        camera.setMoveDirection(scrollDirectionVector(event.scrollDirection));
      break;
  }

  finishTargetSelection.reset();
  goToMainAbilityPage.reset();
  return std::nullopt;
}

void ui::Player::cycleBase(const rts::World& w) {
  const auto baseType{w[side].baseType()};
  const rts::Unit *first{}, *next{};
  int cnt{++lastCycledBase_};
  for (auto& u : w.units) {
    if (u.type == baseType && u.side == side && u.active()) {
      if (!first)
        first = &u;
      if (!cnt--) {
        next = &u;
        break;
      }
    }
  }
  if (!next) {
    next = first;
    lastCycledBase_ = 0;
  }
  if (next)
    camera.setCenter(next->area.center());
}

rts::UnitIdList ui::Player::selectedSameType(const rts::World& w, rts::UnitId unit) const {
  const auto type{w[unit].type};
  return util::filter(w[side].selection().ids(w), [&](rts::UnitId u) { return w[u].type == type; });
}

rts::UnitIdList ui::Player::visibleSameType(const rts::World& w, rts::UnitId unit) const {
  const auto type{w[unit].type};
  return w.unitsInArea(camera.area(), side, type);
}
