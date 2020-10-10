#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Command.h"
#include "rts/types.h"

#include <array>
#include <optional>
#include <utility>

namespace ui {
  struct Player {
    static constexpr size_t MaxCameraPositions{8};

    struct SelectingAbilityTarget {
      rts::AbilityInstanceIndex abilityIndex;
    };

    rts::SideId side;
    Camera camera;
    rts::AbilityPage abilityPage{0};
    std::array<std::optional<rts::Point>, MaxCameraPositions> cameraPositions_;
    std::optional<rts::Rectangle> selectionBox;
    std::optional<SelectingAbilityTarget> selectingAbilityTarget;

    Player(rts::SideId s, Camera&& c) : side{s}, camera{std::move(c)} {}
    void update(const rts::World& w);
    std::optional<rts::Command> processInput(const rts::World& w, const InputEvent& event);

  private:
    enum class State { Default, BuildingPrototype, BuildTriggered };

    rts::Point selectionBoxStart_;
    State state_{State::Default};
    rts::AbilityInstanceIndex lastBuildAbilityIndex_;

    std::optional<rts::Command> doProcessInput(const rts::World& w, const InputEvent& event);
    rts::UnitIdList visibleSameType(const rts::World& w, rts::UnitId unit);
  };
}
