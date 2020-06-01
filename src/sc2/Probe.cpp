#include "sc2/Probe.h"

#include "rts/abilities.h"

sc2::Probe::Probe(rts::Point p) : Inherited{p, rts::Vector{1, 1}} {
  addAbility(rts::abilities::move());
}
