#include "sc2/ui.h"

#include "sc2/Assets.h"

int sc2::ui::Entity::defaultColor(const rts::Entity&) const {
  return sideColor;
}

const ::ui::Sprite& sc2::ui::Geyser::sprite(const rts::ResourceField&) const {
  static const auto& sprite{Assets::getSprite("geyser")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::MineralPatch::sprite(const rts::ResourceField&) const {
  static const auto& sprite{Assets::getSprite("mineral")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Nexus::sprite(const rts::Entity&) const {
  static const auto& sprite{Assets::getSprite("nexus")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::Entity&) const {
  static const auto& sprite{Assets::getSprite("probe")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Rock::sprite(const rts::Blocker&) const {
  static const auto& sprite{Assets::getSprite("rock")};
  return sprite;
}
