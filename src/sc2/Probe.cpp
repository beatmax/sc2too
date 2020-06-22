#include "sc2/Probe.h"

#include "rts/abilities.h"

sc2::Probe::Probe(rts::Point p, rts::SideCPtr sd) : Inherited{p, rts::Vector{1, 1}, sd} {
  addAbility(rts::abilities::move(rts::Speed{4}));
}
