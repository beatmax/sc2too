#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "rts/ResourceField.h"
#include "ui/Sprite.h"

namespace sc2::ui {
  struct Geyser : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::ResourceField&) const final;
  };

  struct MineralPatch : ::ui::SpriteUi<rts::ResourceField> {
    const ::ui::Sprite& sprite(const rts::ResourceField&) const final;
  };

  struct Nexus : ::ui::SpriteUi<rts::Entity> {
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };

  struct Probe : ::ui::SpriteUi<rts::Entity> {
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };

  struct Rock : ::ui::SpriteUi<rts::Blocker> {
    const ::ui::Sprite& sprite(const rts::Blocker&) const final;
  };
}
