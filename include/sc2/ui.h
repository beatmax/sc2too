#pragma once

#include "rts/Blocker.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/Unit.h"
#include "ui/ResourceUi.h"
#include "ui/Sprite.h"

namespace sc2::ui {
  struct Unit : ::ui::SpriteUi<rts::Unit> {
    ::ui::Color sideColor;

    explicit Unit(::ui::Color sc) : sideColor{sc} {}
    ::ui::Color defaultColor(rts::UnitStableRef) const final;
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

  struct AssimilatorType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct GatewayType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct NexusType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct ProbeType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct PylonType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct ZealotType : ::ui::IconUi<rts::UnitType> {
    const ::ui::Icon& icon() const final;
  };

  struct Assimilator : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Gateway : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Nexus : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Probe : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Pylon : Unit {
    using Unit::Unit;
    const ::ui::Sprite& sprite(const rts::World&, rts::UnitStableRef) const final;
  };

  struct Zealot : Unit {
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

  struct SetRallyPointAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInProbeAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInZealotAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInStructureAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInAssimilatorAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInGatewayAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInNexusAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };

  struct WarpInPylonAbility : ::ui::IconUi<rts::Ability> {
    const ::ui::Icon& icon() const final;
  };
}
