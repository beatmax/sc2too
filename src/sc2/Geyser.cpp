#include "sc2/Geyser.h"

#include "sc2/Resources.h"

sc2::Geyser::Geyser(rts::Point p) : Inherited{p, rts::Vector{3, 3}, Resources::gas(), 2250} {
}
