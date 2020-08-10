#include "rts/Side.h"

#include "rts/World.h"

void rts::Side::exec(const World& w, const Command& cmd) {
  std::visit([&](auto&& c) { exec(w, c); }, cmd);
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
