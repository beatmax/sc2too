#include "rts/Side.h"

#include "rts/World.h"
#include "util/algorithm.h"

#include <cassert>

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

rts::UnitId rts::Side::takePrototype() {
  assert(prototype_);
  auto p{prototype_};
  prototype_ = {};
  prototypeBuilderType_ = {};
  return p;
}

rts::WorldActionList rts::Side::exec(const World& w, const Command& cmd) {
  WorldActionList actions;

  if (prototype_) {
    bool keepPrototype{false};
    if (auto* triggerCmd{std::get_if<command::TriggerAbility>(&cmd)}) {
      if (auto t{selection_.subgroupType(w)}) {
        if (w[t].abilities[triggerCmd->abilityIndex].kind == abilities::Kind::Build)
          keepPrototype = true;
      }
    }
    if (!keepPrototype)
      actions += [side{w.id(*this)}](World& w) { w[side].destroyPrototype(w); };
  }

  std::visit([&](auto&& c) { exec(w, actions, c); }, cmd);
  return actions;
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
        groups_[g].remove(selection_.ids(w));
    }
  }
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::BuildPrototype& cmd) {
  actions += [side{w.id(*this)}, t{cmd.unitType}](World& w) {
    auto& s{w[side]};
    auto builderType{s.selection().subgroupType(w)};
    assert(builderType);
    s.createPrototype(w, t, builderType);
  };
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::Cancel& cmd) {
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
      selection_.remove(cmd.units);
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
  actions += [t{selection_.subgroupType(w)}, ids{selection_.ids(w)}, abilityIndex{cmd.abilityIndex},
              target{cmd.target}](World& w) {
    assert(t);
    const auto& ai{w[t].abilities[abilityIndex]};
    assert(ai.kind != abilities::Kind::None);
    if (ai.groupMode == abilities::GroupMode::One) {
      auto it{util::findIf(ids, [&](UnitId id) {
        return !Unit::abilityState(UnitStableRef{w[id]}, w, ai.kind).active();
      })};
      if (it == ids.end())
        it = ids.begin();

      w[*it].trigger(abilityIndex, w, target);
    }
    else {
      for (auto u : ids)
        w[u].trigger(abilityIndex, w, target);
    }
  };
}

void rts::Side::exec(
    const World& w, WorldActionList& actions, const command::TriggerDefaultAbility& cmd) {
  actions += [side{w.id(*this)}, ids{selection_.ids(w)}, target{cmd.target}](World& w) {
    for (auto u : ids) {
      auto& unit{w[u]};
      const auto& unitType{w.unitTypes[unit.type]};
      auto rc{w.relativeContent(side, target)};
      if (auto ai{unitType.defaultAbility[uint32_t(rc)]}; ai != AbilityInstanceIndex::None)
        unit.trigger(ai, w, target);
    }
  };
}

void rts::Side::exec(const World& w, WorldActionList& actions, const command::Debug& cmd) {
  if (cmd.action == command::Debug::Destroy) {
    actions += [ids{selection_.ids(w)}](World& w) {
      for (auto u : ids)
        w.destroy(u);
    };
  }
}

void rts::Side::onMapLoaded(World& w) {
  prototypeMap_.initCells(w.map.size());
}

void rts::Side::onUnitDestroyed(World& w) {
  if (prototype_ && selection().subgroupType(w) != prototypeBuilderType_)
    destroyPrototype(w);
}
