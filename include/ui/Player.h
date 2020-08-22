#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "rts/Command.h"
#include "rts/types.h"

#include <optional>
#include <utility>

namespace ui {
  struct Player {
    rts::SideId side;
    Camera camera;
    std::optional<rts::Rectangle> selectionBox;

    Player(rts::SideId s, Camera&& c) : side{s}, camera{std::move(c)} {}
    std::optional<rts::Command> processInput(const rts::World& w, const InputEvent& event);

  private:
    rts::Point selectionBoxStart_;

    rts::UnitIdList visibleSameType(const rts::World& w, rts::UnitId unit);
  };
}
