#include "SetRallyPoint.h"

#include "rts/Unit.h"
#include "rts/World.h"

void rts::abilities::state::SetRallyPoint::trigger(
    World& w, Unit& u, const Desc& desc, const AbilityTarget& target) {
  w[u.productionQueue].setRallyPoint(w.center(target));
}
