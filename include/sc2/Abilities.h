#pragma once

#include "rts/constants.h"
#include "rts/types.h"

namespace sc2 {
  struct Abilities {
    static void init(rts::World& w);

    static rts::AbilityId chronoBoost;
    static rts::AbilityId gather;
    static rts::AbilityId move;
    static rts::AbilityId setRallyPoint;
    static rts::AbilityId warpInProbe;
    static rts::AbilityId warpInZealot;

    static rts::AbilityId warpInStructure;
    static rts::AbilityId warpInAssimilator;
    static rts::AbilityId warpInGateway;
    static rts::AbilityId warpInNexus;
    static rts::AbilityId warpInPylon;

    static constexpr rts::AbilityPage WarpInStructurePage{1};
    static constexpr auto WarpInStructurePageBase{
        WarpInStructurePage * rts::MaxUnitAbilitiesPerPage};

    static constexpr rts::AbilityInstanceIndex ChronoBoostIndex{10};
    static constexpr rts::AbilityInstanceIndex GatherIndex{5};
    static constexpr rts::AbilityInstanceIndex MoveIndex{0};
    static constexpr rts::AbilityInstanceIndex SetRallyPointIndex{9};
    static constexpr rts::AbilityInstanceIndex WarpInProbeIndex{0};
    static constexpr rts::AbilityInstanceIndex WarpInZealotIndex{0};

    static constexpr rts::AbilityInstanceIndex WarpInStructureIndex{10};
    static constexpr rts::AbilityInstanceIndex WarpInAssimilatorIndex{WarpInStructurePageBase + 1};
    static constexpr rts::AbilityInstanceIndex WarpInGatewayIndex{WarpInStructurePageBase + 5};
    static constexpr rts::AbilityInstanceIndex WarpInNexusIndex{WarpInStructurePageBase + 0};
    static constexpr rts::AbilityInstanceIndex WarpInPylonIndex{WarpInStructurePageBase + 2};
  };
}
