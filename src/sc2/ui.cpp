#include "sc2/ui.h"

#include "sc2/Assets.h"

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::Entity&) const {
  static const auto& sprite{Assets::getSprite("probe")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Rock::sprite(const rts::Blocker&) const {
  static const auto& sprite{Assets::getSprite("rock")};
  return sprite;
}
