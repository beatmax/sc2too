#pragma once

#include "ui/Sprite.h"

namespace sc2::ui {
  struct Probe : ::ui::SpriteUi<rts::Entity> {
    const ::ui::Sprite& sprite(const rts::Entity&) const final;
  };
}
