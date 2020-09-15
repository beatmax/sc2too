#include "rts/abilities.h"

#include "Gather.h"
#include "Move.h"
#include "Produce.h"
#include "SetRallyPoint.h"

int rts::abilities::mutualCancelGroup(Kind kind) {
  switch (kind) {
    case Kind::Gather:
    case Kind::Move:
      return 1;
    default:
      return 0;
  }
}

rts::abilities::Instance rts::abilities::instance(const Gather& desc, AbilityStateIndex as) {
  return Instance{Gather::kind, desc.id, as, state::Gather::makeTrigger(desc)};
}

rts::abilities::Instance rts::abilities::instance(const Move& desc, AbilityStateIndex as) {
  return Instance{Move::kind, desc.id, as, state::Move::makeTrigger(desc)};
}

rts::abilities::Instance rts::abilities::instance(const Produce& desc, AbilityStateIndex as) {
  return Instance{Produce::kind, desc.id, as, state::Produce::makeTrigger(desc)};
}

rts::abilities::Instance rts::abilities::instance(const SetRallyPoint& desc, AbilityStateIndex as) {
  return Instance{SetRallyPoint::kind, desc.id, as, state::SetRallyPoint::makeTrigger(desc)};
}
