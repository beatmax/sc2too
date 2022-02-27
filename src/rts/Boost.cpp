#include "Boost.h"

#include "rts/Unit.h"
#include "rts/World.h"

void rts::abilities::state::Boost::trigger(
    World& w, Unit& u, const Desc& desc, const AbilityTarget& target) {
  if (auto* pId{std::get_if<UnitId>(&target)}) {
    auto& tgt{w[*pId]};
    if (tgt.productionQueue) {
      auto& pq{w[tgt.productionQueue]};
      if (u.consumeEnergy(w, desc.energyCost))
        pq.boost(w, desc.speedUp, desc.duration);
    }
  }
}
