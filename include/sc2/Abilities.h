#pragma once

#include "rts/constants.h"
#include "rts/types.h"

namespace sc2 {
  struct Abilities {
    static void init(rts::World& w);

    static rts::AbilityId chronoBoost;
    static rts::AbilityId gather;
    static rts::AbilityId move;
    static rts::AbilityId researchCharge;
    static rts::AbilityId researchWarpGate;
    static rts::AbilityId setRallyPoint;

    static rts::AbilityId warpInImmortal;
    static rts::AbilityId warpInObserver;
    static rts::AbilityId warpInPrism;
    static rts::AbilityId warpInProbe;
    static rts::AbilityId warpInStalker;
    static rts::AbilityId warpInZealot;

    static rts::AbilityId warpInStructure;
    static rts::AbilityId warpInAssimilator;
    static rts::AbilityId warpInCyberCore;
    static rts::AbilityId warpInGateway;
    static rts::AbilityId warpInNexus;
    static rts::AbilityId warpInPylon;

    static rts::AbilityId warpInStructure2;
    static rts::AbilityId warpInRobo;
    static rts::AbilityId warpInTemplar;
    static rts::AbilityId warpInTwilight;

    static constexpr rts::AbilityPage WarpInStructurePage{1};
    static constexpr auto WarpInStructurePageBase{
        WarpInStructurePage * rts::MaxUnitAbilitiesPerPage};

    static constexpr rts::AbilityPage WarpInStructure2Page{2};
    static constexpr auto WarpInStructure2PageBase{
        WarpInStructure2Page * rts::MaxUnitAbilitiesPerPage};

    static constexpr rts::AbilityInstanceIndex ChronoBoostIndex{10};
    static constexpr rts::AbilityInstanceIndex GatherIndex{5};
    static constexpr rts::AbilityInstanceIndex MoveIndex{0};
    static constexpr rts::AbilityInstanceIndex ResearchChargeIndex{0};
    static constexpr rts::AbilityInstanceIndex ResearchWarpGateIndex{10};
    static constexpr rts::AbilityInstanceIndex SetRallyPointIndex{9};

    static constexpr rts::AbilityInstanceIndex WarpInImmortalIndex{2};
    static constexpr rts::AbilityInstanceIndex WarpInObserverIndex{0};
    static constexpr rts::AbilityInstanceIndex WarpInPrismIndex{1};
    static constexpr rts::AbilityInstanceIndex WarpInProbeIndex{0};
    static constexpr rts::AbilityInstanceIndex WarpInStalkerIndex{2};
    static constexpr rts::AbilityInstanceIndex WarpInZealotIndex{0};

    static constexpr rts::AbilityInstanceIndex WarpInStructureIndex{10};
    static constexpr rts::AbilityInstanceIndex WarpInAssimilatorIndex{WarpInStructurePageBase + 1};
    static constexpr rts::AbilityInstanceIndex WarpInCyberCoreIndex{WarpInStructurePageBase + 10};
    static constexpr rts::AbilityInstanceIndex WarpInGatewayIndex{WarpInStructurePageBase + 5};
    static constexpr rts::AbilityInstanceIndex WarpInNexusIndex{WarpInStructurePageBase + 0};
    static constexpr rts::AbilityInstanceIndex WarpInPylonIndex{WarpInStructurePageBase + 2};

    static constexpr rts::AbilityInstanceIndex WarpInStructure2Index{11};
    static constexpr rts::AbilityInstanceIndex WarpInTwilightIndex{WarpInStructure2PageBase + 0};
    static constexpr rts::AbilityInstanceIndex WarpInRoboIndex{WarpInStructure2PageBase + 2};
    static constexpr rts::AbilityInstanceIndex WarpInTemplarIndex{WarpInStructure2PageBase + 5};
  };
}
