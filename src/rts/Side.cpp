#include "rts/Side.h"

#include "rts/World.h"

#include <cassert>

void rts::Side::exec(const World& w, const Command& cmd) {
  std::visit([&](auto&& c) { exec(w, c); }, cmd);
}

void rts::Side::exec(const World& w, const command::ControlGroup& cmd) {
  assert(cmd.group < MaxControlGroups);
  Selection& group{groups_[cmd.group]};

  switch (cmd.action) {
    case command::ControlGroup::Select:
      selection_ = group;
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

void rts::Side::exec(const World& w, const command::Selection& cmd) {
  switch (cmd.action) {
    case command::Selection::Set:
      selection_.set(w, cmd.entities);
      break;
    case command::Selection::Add:
      selection_.add(w, cmd.entities);
      break;
    case command::Selection::Remove:
      selection_.remove(cmd.entities);
      break;
  }
}

void rts::Side::exec(const World& w, const command::TriggerAbility& cmd) {
  for (auto entity : selection_.items(w))
    entity->trigger(cmd.ability, w, cmd.target);
}

void rts::Side::exec(const World& w, const command::TriggerDefaultAbility& cmd) {
  for (auto entity : selection_.items(w)) {
    const auto& entityType{w.entityTypes[entity->type]};
    auto rc{w.relativeContent(w.sides.id(*this), cmd.target)};
    if (auto a{entityType.defaultAbility[size_t(rc)]})
      entity->trigger(a, w, cmd.target);
  }
}
