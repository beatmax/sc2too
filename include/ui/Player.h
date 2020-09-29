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

    rts::SideId side;
    Camera camera;
    std::array<std::optional<rts::Point>, MaxCameraPositions> cameraPositions_;
    std::optional<rts::Rectangle> selectionBox;
    std::optional<rts::AbilityId> selectingAbilityTarget;

    Player(rts::SideId s, Camera&& c) : side{s}, camera{std::move(c)} {}
    std::optional<rts::Command> processInput(const rts::World& w, const InputEvent& event);

  private:
    rts::Point selectionBoxStart_;

    std::optional<rts::Command> doProcessInput(const rts::World& w, const InputEvent& event);
    rts::UnitIdList visibleSameType(const rts::World& w, rts::UnitId unit);
  };
}
