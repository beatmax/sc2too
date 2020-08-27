#pragma once

#include "rts/Blocker.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/Unit.h"
#include "ui/ResourceUi.h"
#include "ui/Sprite.h"

namespace sc2::ui {
  struct Unit : ::ui::SpriteUi<rts::Unit> {
    int sideColor;

    explicit Unit(int sc) : sideColor{sc} {}
    int defaultColor(rts::UnitStableRef) const final;
  };

  struct Mineral : ::ui::ResourceUi {
    const ::ui::Icon& icon() const final;
    const char* msgMoreRequired() const final;
    const char* msgCapReached() const final { return ""; }
  };

  struct Gas : ::ui::ResourceUi {
    const ::ui::Icon& icon() const final;
    const char* msgMoreRequired() const final;
    const char* msgCapReached() const final { return ""; }
  };

  struct Supply : ::ui::ResourceUi {
    Supply() : ::ui::ResourceUi{::ui::ResourceUi::Display::Allocated} {}
    const ::ui::Icon& icon() const final;
    const char* msgMoreRequired() const final;
    const char* msgCapReached() const final;
  };

  struct Geyser : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::World&, rts::ResourceFieldStableRef) const final;
  };

  struct MineralPatch : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::World&, rts::ResourceFieldStableRef) const final;
  };

  struct NexusType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct ProbeType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct Nexus : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Probe : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Rock : ::ui::SpriteUi<rts::Blocker> {
    const ::ui::Sprite& sprite(const rts::World&, rts::BlockerStableRef) const final;
  };

  struct GatherAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct MoveAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInProbeAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };
}
