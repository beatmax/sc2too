#include "rts/Side.h"

#include "rts/World.h"

void rts::Side::exec(const World& world, const Command& cmd) {
  std::visit([&](auto&& c) { exec(world, c); }, cmd);
}

void rts::Side::exec(const World& world, const command::TriggerAbility& cmd) {
  for (auto entity : selection_.items(world)) {
    const auto& entityType{world.entityTypes[entity->type]};
    entity->trigger(cmd.ability, world, cmd.target);
  }
}

void rts::Side::exec(const World& world, const command::TriggerDefaultAbility& cmd) {
  if (auto st{selection_.subselectionType(world)}) {
    const auto& entityType{world.entityTypes[st]};
    auto rc{world.relativeContent(world.sides.id(*this), cmd.target)};
    if (auto a{entityType.defaultAbility[size_t(rc)]})
      exec(world, command::TriggerAbility{a, cmd.target});
  }
}

void rts::Side::exec(const World& world, const command::Selection& cmd) {
  switch (cmd.action) {
    case command::Selection::Set:
      selection_.set(world, cmd.entities);
      break;
    case command::Selection::Add:
      selection_.add(world, cmd.entities);
      break;
    case command::Selection::Remove:
      selection_.remove(cmd.entities);
      break;
  }
}
