#include "SetRallyPoint.h"

#include "rts/Unit.h"
#include "rts/World.h"

void rts::abilities::state::SetRallyPoint::trigger(
    World& w, Unit& u, const Desc& desc, const AbilityTarget& target) {
  assert(std::holds_alternative<Point>(target));
  w[u.productionQueue].setRallyPoint(std::get<Point>(target));
}
