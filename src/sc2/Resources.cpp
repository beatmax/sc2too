#include "sc2/Resources.h"

#include "rts/constants.h"
#include "sc2/ui.h"

#include <array>
#include <memory>

namespace sc2 {
  namespace {
    // resources are ordered by address
    const std::array<rts::Resource, 3> resources{rts::Resource{std::make_unique<ui::Mineral>()},
                                                 rts::Resource{std::make_unique<ui::Gas>()},
                                                 rts::Resource{std::make_unique<ui::Supply>()}};
  }
}

rts::ResourceCPtr sc2::Resources::mineral() {
  return &resources[0];
}

rts::ResourceCPtr sc2::Resources::gas() {
  return &resources[1];
}

rts::ResourceCPtr sc2::Resources::supply() {
  return &resources[2];
}

rts::ResourceBank sc2::Resources::initialResources() {
  return {{mineral(), 0, rts::QuantityInf},
          {gas(), 0, rts::QuantityInf},
          {supply(), 0, rts::QuantityInf}};
}
