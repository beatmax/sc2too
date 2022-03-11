#include "rts/Side.h"

#include "rts/World.h"
#include "util/algorithm.h"

#include <cassert>
#include <utility>

void rts::Side::updateActiveUnitCount(UnitTypeId type, int16_t inc) {
  activeUnitCounts_[type.idx] += inc;
}

void rts::Side::addUpgrade(UpgradeId u) {
  setUpgradeInResearch(u, false);
  upgrades_[u.idx] = true;
}

void rts::Side::createPrototype(World& w, UnitTypeId t, UnitTypeId builderType) {
  assert(!prototype_);

  if (auto [result, failedResource] =
          resources_.allocate(w[t].cost, AllocFilter::Any, AllocMode::CheckOnly);
      result == AllocResult::Success) {
    prototype_ = w.createUnit(t, w.id(*this));
    prototypeBuilderType_ = builderType;
  }
  else {
    auto& ui{w[failedResource].ui()};
    messages_.add(w, result == AllocResult::Lack ? ui.msgMoreRequired() : ui.msgCapReached());
  }
}

void rts::Side::destroyPrototype(World& w) {
  assert(prototype_);
  w.destroy(takePrototype());
}

bool rts::Side::materializePrototype(World& w, Point p) {
  assert(prototype_);
  auto& proto{w[prototype_]};
  auto& type{w[proto.type]};
  auto buildArea{rectangleCenteredAt(p, proto.area.size, w.map.area())};
  bool invalidLocation{false};
  if (!prototypeMap_.isEmpty(buildArea)) {
    invalidLocation = true;
  }
  else if (type.extractedResource) {
    if (auto* rf{w.resourceField(p)}; rf && rf->bag.resource == type.extractedResource) {
      buildArea.topLeft = rf->area.topLeft;
      proto.resourceField = w.id(*rf);
    }
    else {
      invalidLocation = true;
    }
  }
  else if (
      !w.map.isEmpty(buildArea) ||
      (proto.type == baseType_ && util::anyOf(buildArea.points(), [&](Point p) {
         return w.resourceProximityMap.isActive(p);
       }))) {
    invalidLocation = true;
  }
  if (invalidLocation) {
    messages_.add(w, "INVALID LOCATION!");
    return false;
  }
  if (type.requiresPower == UnitType::RequiresPower::Yes &&
      !powerMap_.isActive(buildArea.center())) {
    messages_.add(w, "YOU MUST PLACE THAT IN A POWER FIELD!");
    return false;
  }
  if (!proto.tryAllocate(w))
    return false;
  proto.setBuildPoint(w, buildArea.topLeft);
  return true;
}

rts::UnitId rts::Side::takePrototype() {
  assert(prototype_);
  prototypeBuilderType_ = {};
  return std::exchange(prototype_, {});
}

rts::UnitId rts::Side::takeAndCreatePrototype(World& w) {
  assert(prototype_);
  return std::exchange(prototype_, w.createUnit(w[prototype_].type, w.id(*this)));
}

rts::WorldActionList rts::Side::exec(const World& w, const Command& cmd) {
  WorldActionList actions;

  if (prototype_) {
    bool keepPrototype{false};
    if (auto* triggerCmd{std::get_if<command::TriggerAbility>(&cmd)}) {
      const auto* a{selection_.subgroup(w).abilities[triggerCmd->abilityIndex]};
      if (a && a->kind == abilities::Kind::Build)
        keepPrototype = true;
    }
    if (!keepPrototype)
      actions += [side{w.id(*this)}](World& w) { w[side].destroyPrototype(w); };
  }

  std::visit([&](auto&& c) { exec(w, actions, c); }, cmd);
  return actions;
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::Cancel& cmd) {
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::ControlGroup& cmd) {
  assert(cmd.group < MaxControlGroups);
  Selection& group{groups_[cmd.group]};

  switch (cmd.action) {
    case command::ControlGroup::Select:
      selection_.set(w, group.ids(w));
      break;
    case command::ControlGroup::Set:
      group = selection_;
      break;
    case command::ControlGroup::Add:
      group.add(w, selection_.ids(w));
      break;
  }

  if (cmd.exclusive) {
    for (ControlGroupId g{0}; g < MaxControlGroups; ++g) {
      if (g != cmd.group)
        groups_[g].remove(w, selection_.ids(w));
    }
  }
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::PrepareAbility& cmd) {
  const auto& subgroup{selection_.subgroup(w)};
  const auto* a{subgroup.abilities[cmd.abilityIndex]};
  assert(a);
  assert(a->kind != abilities::Kind::None);

  preparedAbilityIndex_ = AbilityInstanceIndex::None;

  UnitIdList ids = util::filter(selection_.ids(w), [&](UnitId id) {
    return w[id].abilityReadyState(w, cmd.abilityIndex, a->abilityId) == AbilityReadyState::Ready;
  });
  if (ids.empty())
    return;

  if (a->energyCost) {
    assert(a->groupMode == abilities::GroupMode::One);
    if (util::noneOf(ids, [&w, cost{a->energyCost}](UnitId id) { return w[id].energy >= cost; })) {
      actions += [side{w.id(*this)}](World& w) { w[side].messages().add(w, "NOT ENOUGH ENERGY!"); };
      return;
    }
  }

  if (a->kind == rts::abilities::Kind::Build) {
    auto builderType{subgroup.type};
    if (!builderType)
      return;
    actions += [side{w.id(*this)}, t{a->desc<rts::abilities::Build>().type}, builderType,
                ai{cmd.abilityIndex}](World& w) {
      auto& s{w[side]};
      s.createPrototype(w, t, builderType);
      if (s.prototype())
        s.preparedAbilityIndex_ = ai;
    };
  }
  else {
    preparedAbilityIndex_ = cmd.abilityIndex;
  }
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::Selection& cmd) {
  switch (cmd.action) {
    case command::Selection::Set:
      selection_.set(w, cmd.units);
      break;
    case command::Selection::Add:
      selection_.add(w, cmd.units);
      break;
    case command::Selection::Remove:
      selection_.remove(w, cmd.units);
      break;
  }
}

void rts::Side::exec(
    const World& w, WorldActionList& actions, const command::SelectionSubgroup& cmd) {
  switch (cmd.action) {
    case command::SelectionSubgroup::Next:
      selection_.subgroupNext(w);
      break;
    case command::SelectionSubgroup::Previous:
      selection_.subgroupPrevious(w);
      break;
  }
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::TriggerAbility& cmd) {
  const auto* a{selection_.subgroup(w).abilities[cmd.abilityIndex]};
  assert(a);
  assert(a->kind != abilities::Kind::None);

  const auto sideId{w.id(*this)};
  if (a->targetType != abilities::TargetType::None &&
      !w.compatibleTarget(a->targetType, sideId, cmd.target)) {
    return;
  }
  ++triggerCount_;  // causes UI to stop selecting target

  UnitIdList ids = util::filter(selection_.ids(w), [&](UnitId id) {
    return w[id].abilityReadyState(w, cmd.abilityIndex, a->abilityId) == AbilityReadyState::Ready;
  });
  if (ids.empty())
    return;

  if (a->groupMode == abilities::GroupMode::One) {
    UnitId uId;
    if (a->energyCost)
      uId = *util::minElementBy(ids, [&w](UnitId id) { return -w[id].energy; });
    else if (a->kind == abilities::Kind::Produce) {
      uId = *util::minElementBy(ids, [&w](UnitId id) { return w[w[id].productionQueue].size(); });
    }
    else {
      uId = *util::minElementBy(ids, [&w, a](UnitId id) {
        auto& u{w[id]};
        return u.commandQueue.size() +
            (Unit::abilityState(UnitStableRef{u}, w, a->kind).active() ? 1000 : 0);
      });
    }
    ids = {uId};
  }

  actions +=
      [sideId, ids{std::move(ids)},
       cmd{command::TriggerAbility{cmd.abilityIndex, cmd.target, cmd.enqueue && a->enqueable}},
       isBuild{a->kind == abilities::Kind::Build}](World& w) {
        auto& side = w[sideId];

        rts::UnitId protoId;
        if (isBuild) {
          assert(ids.size() == 1);
          if (!side.materializePrototype(w, cmd.target))
            return;
          protoId = cmd.enqueue ? side.takeAndCreatePrototype(w) : side.takePrototype();
        }

        if (cmd.enqueue) {
          UnitCommand ucmd{
              cmd.abilityIndex, w.abilityWeakTarget(cmd.target), w.nextTriggerGroupId++,
              uint32_t(ids.size()), protoId};
          for (auto u : ids) {
            auto& unit = w[u];
            if (!unit.commandQueue.full())
              unit.commandQueue.push(ucmd);
            else if (protoId)
              w.destroy(protoId);
          }
        }
        else {
          auto target{w.abilityTarget(cmd.target)};
          TriggerGroup group{uint32_t(ids.size())};
          for (auto u : ids) {
            auto& unit = w[u];
            unit.clearCommandQueue(w);
            unit.trigger(cmd.abilityIndex, w, group, target, protoId);
            assert(  // trigger build always succeeds (prototype is not leaked)
                !protoId ||
                Unit::abilityState<abilities::BuildState>(UnitStableRef{unit}, w) ==
                    abilities::BuildState::Init);
          }
        }
      };
}

void rts::Side::exec(
    const World& w, WorldActionList& actions, const command::TriggerDefaultAbility& cmd) {
  const auto& subgroup{selection_.subgroup(w)};

  auto rc{cmd.minimap ? RelativeContent::Ground : w.relativeContent(w.id(*this), cmd.target)};
  auto abilityIndex{subgroup.defaultAbility[uint32_t(rc)]};
  if (abilityIndex == AbilityInstanceIndex::None)
    return;

  exec(w, actions, command::TriggerAbility{abilityIndex, cmd.target, cmd.enqueue});
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::Debug& cmd) {
  if (cmd.action == command::Debug::Destroy) {
    actions += [ids{selection_.ids(w)}](World& w) {
      for (auto u : ids)
        w.destroy(u);
    };
  }
}

void rts::Side::onMapCreated(World& w) {
  powerMap_.initCells(w.map.size());
  prototypeMap_.initCells(w.map.size());
}

void rts::Side::onUnitDestroyed(World& w) {
  if (prototype_ && selection().subgroup(w).type != prototypeBuilderType_)
    destroyPrototype(w);
}
