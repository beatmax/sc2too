#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "ui/Sprite.h"

namespace sc2::ui {
  struct Probe : ::ui::SpriteUi<rts::Entity> {
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };

  struct Rock : ::ui::SpriteUi<rts::Blocker> {
    const ::ui::Sprite& sprite(const rts::Blocker&) const final;
  };
}
