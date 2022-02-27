#include "sc2/ui.h"

#include "rts/World.h"
#include "rts/abilities.h"
#include "sc2/Abilities.h"
#include "sc2/Assets.h"
#include "sc2/Resources.h"
#include "ui/fx.h"

ui::Color sc2::ui::Unit::defaultColor(rts::UnitStableRef) const {
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

const char* sc2::ui::Supply::msgCapReached() const {
  return "WE ARE AT MAXIMUM SUPPLY!";
}

const ::ui::Sprite& sc2::ui::Geyser::sprite(
    const rts::World&, rts::ResourceFieldStableRef rf) const {
  static const auto& sprite{Assets::getSprite("geyser")};
  static const auto& spriteEmpty{Assets::getSprite("geyser_empty")};
  return rf->bag.empty() ? spriteEmpty : sprite;
}

const ::ui::Sprite& sc2::ui::MineralPatch::sprite(
    const rts::World&, rts::ResourceFieldStableRef) const {
  static const auto& sprite{Assets::getSprite("mineral")};
  return sprite;
}

const ::ui::Icon& sc2::ui::AssimilatorType::icon() const {
  static const auto& icon{Assets::getIcon("assimilator")};
  return icon;
}

const ::ui::Icon& sc2::ui::GatewayType::icon() const {
  static const auto& icon{Assets::getIcon("gateway")};
  return icon;
}

const ::ui::Icon& sc2::ui::NexusType::icon() const {
  static const auto& icon{Assets::getIcon("nexus")};
  return icon;
}

const ::ui::Icon& sc2::ui::ProbeType::icon() const {
  static const auto& icon{Assets::getIcon("probe")};
  return icon;
}

const ::ui::Icon& sc2::ui::PylonType::icon() const {
  static const auto& icon{Assets::getIcon("pylon")};
  return icon;
}

const ::ui::Icon& sc2::ui::ZealotType::icon() const {
  static const auto& icon{Assets::getIcon("zealot")};
  return icon;
}

const ::ui::Sprite& sc2::ui::Assimilator::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using State = rts::Unit::State;

  static const auto& sprite{Assets::getSprite("assimilator")};
  static const auto& spriteBuilding{Assets::getSprite("building3x3")};
  static const auto& spriteEmpty{Assets::getSprite("assimilator_empty")};
  static const auto spritePrototype{::ui::fx::flatten(sprite)};

  switch (u->state) {
    case State::New:
    case State::Allocated:
    case State::Buildable:
      return spritePrototype;
    case State::Building:
      return spriteBuilding;
    case State::Active:
    case State::Destroyed:
      break;
  }
  if (u->resourceField && w[u->resourceField].bag.empty())
    return spriteEmpty;
  return sprite;
}

const ::ui::Sprite& sc2::ui::Gateway::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using State = rts::Unit::State;
  using ProduceState = rts::abilities::ProduceState;

  static const auto& sprite{Assets::getSprite("gateway")};
  static const auto& spriteBuilding{Assets::getSprite("building3x3")};
  static const auto& spriteProducing{Assets::getSprite("gateway_producing")};
  static const auto spritePrototype{::ui::fx::flatten(sprite)};
  static const auto spriteUnpowered{::ui::fx::unpower(sprite)};

  switch (u->state) {
    case State::New:
    case State::Allocated:
    case State::Buildable:
      return spritePrototype;
    case State::Building:
      return spriteBuilding;
    case State::Active:
    case State::Destroyed:
      break;
  }
  return !u->powered                                                             ? spriteUnpowered
      : (rts::Unit::abilityState<ProduceState>(u, w) == ProduceState::Producing) ? spriteProducing
                                                                                 : sprite;
}

const ::ui::Sprite* sc2::ui::Gateway::overlay(const rts::World& w, rts::UnitStableRef u) const {
  static const auto& spriteChronoBoost{Assets::getSprite("boost3x3")};
  return w[u->productionQueue].boosted(w) ? &spriteChronoBoost : nullptr;
}

const ::ui::Sprite& sc2::ui::Nexus::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using State = rts::Unit::State;
  using ProduceState = rts::abilities::ProduceState;

  static const auto& sprite{Assets::getSprite("nexus")};
  static const auto& spriteBuilding{Assets::getSprite("building5x5")};
  static const auto& spriteProducing{Assets::getSprite("nexus_producing")};
  static const auto spritePrototype{::ui::fx::flatten(sprite)};

  switch (u->state) {
    case State::New:
    case State::Allocated:
    case State::Buildable:
      return spritePrototype;
    case State::Building:
      return spriteBuilding;
    case State::Active:
    case State::Destroyed:
      break;
  }
  return (rts::Unit::abilityState<ProduceState>(u, w) == ProduceState::Producing) ? spriteProducing
                                                                                  : sprite;
}

const ::ui::Sprite* sc2::ui::Nexus::overlay(const rts::World& w, rts::UnitStableRef u) const {
  static const auto& spriteChronoBoost{Assets::getSprite("boost5x5")};
  return w[u->productionQueue].boosted(w) ? &spriteChronoBoost : nullptr;
}

const ::ui::Sprite& sc2::ui::Probe::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using GatherState = rts::abilities::GatherState;
  static const auto& sprite{Assets::getSprite("probe")};
  static const auto& spriteGather{Assets::getSprite("probe_gather")};
  static const auto& spriteMineral{Assets::getSprite("probe_mineral")};
  static const auto& spriteGas{Assets::getSprite("probe_gas")};
  return (!u->bag.empty()) ? (u->bag.resource == Resources::mineral ? spriteMineral : spriteGas)
      : (rts::Unit::abilityState<GatherState>(u, w) == GatherState::Gathering) ? spriteGather
                                                                               : sprite;
}

const ::ui::Sprite& sc2::ui::Pylon::sprite(const rts::World& w, rts::UnitStableRef u) const {
  using State = rts::Unit::State;

  static const auto& sprite{Assets::getSprite("pylon")};
  static const auto& spriteBuilding{Assets::getSprite("building2x2")};
  static const auto spritePrototype{::ui::fx::flatten(sprite)};

  switch (u->state) {
    case State::New:
    case State::Allocated:
    case State::Buildable:
      return spritePrototype;
    case State::Building:
      return spriteBuilding;
    case State::Active:
    case State::Destroyed:
      break;
  }
  return sprite;
}

const ::ui::Sprite& sc2::ui::Zealot::sprite(const rts::World& w, rts::UnitStableRef u) const {
  static const auto& sprite{Assets::getSprite("zealot")};
  return sprite;
}

const ::ui::Sprite& sc2::ui::Rock::sprite(const rts::World&, rts::BlockerStableRef b) const {
  static const auto& sprite{Assets::getSprite("rock")};
  static const auto& sprite2x2{Assets::getSprite("rock2x2")};
  return b->area.size == rts::Vector{2, 2} ? sprite2x2 : sprite;
}

const ::ui::Icon& sc2::ui::ChronoBoostAbility::icon() const {
  static const auto& icon{Assets::getIcon("boost")};
  return icon;
}

const ::ui::Icon& sc2::ui::GatherAbility::icon() const {
  static const auto& icon{Assets::getIcon("gather")};
  return icon;
}

const ::ui::Icon& sc2::ui::MoveAbility::icon() const {
  static const auto& icon{Assets::getIcon("move")};
  return icon;
}

const ::ui::Icon& sc2::ui::SetRallyPointAbility::icon() const {
  static const auto& icon{Assets::getIcon("rally")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInProbeAbility::icon() const {
  static const auto& icon{Assets::getIcon("probe")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInZealotAbility::icon() const {
  static const auto& icon{Assets::getIcon("zealot")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInStructureAbility::icon() const {
  static const auto& icon{Assets::getIcon("nexus")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInAssimilatorAbility::icon() const {
  static const auto& icon{Assets::getIcon("assimilator")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInGatewayAbility::icon() const {
  static const auto& icon{Assets::getIcon("gateway")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInNexusAbility::icon() const {
  static const auto& icon{Assets::getIcon("nexus")};
  return icon;
}

const ::ui::Icon& sc2::ui::WarpInPylonAbility::icon() const {
  static const auto& icon{Assets::getIcon("pylon")};
  return icon;
}
