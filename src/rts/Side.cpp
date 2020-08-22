#include "rts/Side.h"

#include "rts/World.h"

#include <cassert>

rts::WorldActionList rts::Side::exec(const World& w, const Command& cmd) {
  WorldActionList actions;
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
  actions += [ids{selection_.ids(w)}, ability{cmd.ability}, target{cmd.target}](World& w) {
    for (auto u : ids)
      w[u].trigger(ability, w, target);
  };
}

void rts::Side::exec(
    const World& w, WorldActionList& actions, const command::TriggerDefaultAbility& cmd) {
  actions += [side{w.id(*this)}, ids{selection_.ids(w)}, target{cmd.target}](World& w) {
    for (auto u : ids) {
      auto& unit{w[u]};
      const auto& unitType{w.unitTypes[unit.type]};
      auto rc{w.relativeContent(side, target)};
      if (auto a{unitType.defaultAbility[size_t(rc)]})
        unit.trigger(a, w, target);
    }
  };
}
