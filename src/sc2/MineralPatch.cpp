#include "sc2/MineralPatch.h"

#include "sc2/Resources.h"

sc2::MineralPatch::MineralPatch(rts::Point p)
  : Inherited{p, rts::Vector{1, 1}, Resources::mineral(), 1800} {
}
