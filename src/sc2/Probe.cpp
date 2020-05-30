#include "sc2/Probe.h"

#include "rts/abilities.h"

sc2::Probe::Probe(rts::Position p) : Inherited{p} {
  addAbility(rts::abilities::move());
}
