#include "sc2/Nexus.h"

#include "rts/abilities.h"

sc2::Nexus::Nexus(rts::Point p, rts::SideCPtr sd) : Inherited{p, rts::Vector{5, 5}, sd} {
}
