#include "sc2/Resources.h"

#include "rts/World.h"
#include "rts/constants.h"
#include "sc2/constants.h"
#include "sc2/ui.h"

#include <array>
#include <memory>

void sc2::Resources::init(rts::World& w) {
  mineral = w.createResource(std::make_unique<ui::Mineral>());
  gas = w.createResource(std::make_unique<ui::Gas>());
  supply = w.createResource(std::make_unique<ui::Supply>());
}

rts::ResourceInitList sc2::Resources::initialResources() {
  return {
      {mineral, 50, rts::QuantityInf, rts::ResourceRecoverable::No},
      {gas, 0, rts::QuantityInf, rts::ResourceRecoverable::No},
      {supply, 0, MaxSupplySlots, rts::ResourceRecoverable::Yes}};
}

rts::ResourceId sc2::Resources::mineral;
rts::ResourceId sc2::Resources::gas;
rts::ResourceId sc2::Resources::supply;
