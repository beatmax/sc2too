#include "sc2/ui.h"

#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/Assets.h"

int sc2::ui::Unit::defaultColor(rts::UnitStableRef) const {
  return sideColor;
}

const ::ui::Icon& sc2::ui::Mineral::icon() const {
  static const auto& icon{Assets::getIcon("mineral")};
  return icon;
}

const char* sc2::ui::Mineral::msgMoreRequired() const {
  return "WE REQUIRE MORE MINERALS!";
}

const ::ui::Icon& sc2::ui::Gas::icon() const {
  static const auto& icon{Assets::getIcon("gas")};
  return icon;
}

const char* sc2::ui::Gas::msgMoreRequired() const {
  return "WE REQUIRE MORE VESPENE GAS!";
}

const ::ui::Icon& sc2::ui::Supply::icon() const {
  static const auto& icon{Assets::getIcon("supply")};
  return icon;
}

const char* sc2::ui::Supply::msgMoreRequired() const {
  return "YOU MUST CONSTRUCT ADDITIONAL PYLONS!";
}

const ::ui::Sprite& sc2::ui::Geyser::sprite(const rts::World&, rts::ResourceFieldStableRef) const {
  static const auto& sprite{Assets::getSprite("geyser")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::MineralPatch::sprite(
    const rts::World&, rts::ResourceFieldStableRef) const {
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

const ::ui::Sprite& sc2::ui::Nexus::sprite(const rts::World&, rts::UnitStableRef) const {
  static const auto& sprite{Assets::getSprite("nexus")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using GatherState = rts::abilities::GatherState;
  static const auto& sprite{Assets::getSprite("probe")};
  static const auto& spriteGather{Assets::getSprite("probe_gather")};
  static const auto& spriteMineral{Assets::getSprite("probe_mineral")};
  return (!u->bag.empty())
      ? spriteMineral
      : (rts::Unit::state<GatherState>(u, w) == GatherState::Gathering) ? spriteGather : sprite;
}

const ::ui::Sprite& sc2::ui::Rock::sprite(const rts::World&, rts::BlockerStableRef) const {
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

const ::ui::Icon& sc2::ui::WarpInProbeAbility::icon() const {
  static const auto& icon{Assets::getIcon("probe")};
  return icon;
}
