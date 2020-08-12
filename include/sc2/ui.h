#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "rts/ResourceField.h"
#include "ui/Sprite.h"

namespace sc2::ui {
  struct Entity : ::ui::SpriteUi<rts::Entity> {
    int sideColor;

    explicit Entity(int sc) : sideColor{sc} {}
    int defaultColor(const rts::Entity&) const final;
  };

  struct Geyser : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::ResourceField&) const final;
  };

  struct MineralPatch : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::ResourceField&) const final;
  };

  struct NexusType : ::ui::IconUi<rts::EntityType> {
    const ::ui::Icon& icon() const final;
  };

  struct ProbeType : ::ui::IconUi<rts::EntityType> {
    const ::ui::Icon& icon() const final;
  };

  struct Nexus : Entity {
    using Entity::Entity;
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };

  struct Probe : Entity {
    using Entity::Entity;
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };

  struct Rock : ::ui::SpriteUi<rts::Blocker> {
    const ::ui::Sprite& sprite(const rts::Blocker&) const final;
  };

  struct MoveAbility : rts::Ui {};
}
