#include "sc2/ui.h"

#include "rts/abilities.h"
#include "sc2/Abilities.h"
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

const ::ui::Icon& sc2::ui::NexusType::icon() const {
  static const auto& icon{Assets::getIcon("nexus")};
  return icon;
}

const ::ui::Icon& sc2::ui::ProbeType::icon() const {
  static const auto& icon{Assets::getIcon("probe")};
  return icon;
}

const ::ui::Sprite& sc2::ui::Nexus::sprite(const rts::Entity&) const {
  static const auto& sprite{Assets::getSprite("nexus")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::Entity& e) const {
  using GatherState = rts::abilities::GatherState;
  static const auto& sprite{Assets::getSprite("probe")};
  static const auto& spriteGather{Assets::getSprite("probe_gather")};
  static const auto& spriteMineral{Assets::getSprite("probe_mineral")};
  return (!e.bag.empty())
      ? spriteMineral
      : (e.state<GatherState>(Abilities::GatherIndex) == GatherState::Gathering) ? spriteGather
                                                                                 : sprite;
}

const ::ui::Sprite& sc2::ui::Rock::sprite(const rts::Blocker&) const {
  static const auto& sprite{Assets::getSprite("rock")};
  return sprite;
}

const ::ui::Icon& sc2::ui::GatherAbility::icon() const {
  static const auto& icon{Assets::getIcon("gather")};
  return icon;
}

const ::ui::Icon& sc2::ui::MoveAbility::icon() const {
  static const auto& icon{Assets::getIcon("move")};
  return icon;
}
