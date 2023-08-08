#pragma once

#include "rts/Factory.h"

namespace sc2 {
  class Factory : public rts::Factory {
  public:
    void init(rts::World& w) final;
    rts::UnitId create(rts::World& w, rts::UnitTypeId t, rts::SideId sd) final;
    void onProduce(rts::World& w, rts::ProducibleId p, rts::SideId sd) final;

    static rts::UnitId assimilator(rts::World& w, rts::SideId sd, rts::ResourceFieldId rf = {});
    static rts::UnitId cyberCore(rts::World& w, rts::SideId sd);
    static rts::UnitId gateway(rts::World& w, rts::SideId sd);
    static rts::UnitId immortal(rts::World& w, rts::SideId sd);
    static rts::UnitId nexus(rts::World& w, rts::SideId sd);
    static rts::UnitId observer(rts::World& w, rts::SideId sd);
    static rts::UnitId prism(rts::World& w, rts::SideId sd);
    static rts::UnitId probe(rts::World& w, rts::SideId sd);
    static rts::UnitId pylon(rts::World& w, rts::SideId sd);
    static rts::UnitId robo(rts::World& w, rts::SideId sd);
    static rts::UnitId stalker(rts::World& w, rts::SideId sd);
    static rts::UnitId templar(rts::World& w, rts::SideId sd);
    static rts::UnitId twilight(rts::World& w, rts::SideId sd);
    static rts::UnitId zealot(rts::World& w, rts::SideId sd);

    static rts::ResourceFieldId geyser(rts::World& w);
    static rts::ResourceFieldId mineralPatch(rts::World& w, rts::ResourceGroupId group);
    static rts::BlockerId rock(rts::World& w, rts::Vector size);
  };
}
