#pragma once

#include "Camera.h"
#include "InputEvent.h"
#include "WorldView.h"
#include "rts/Command.h"
#include "rts/types.h"

#include <array>
#include <chrono>
#include <optional>
#include <utility>

namespace ui {
  struct Player {
    static constexpr size_t MaxCameraPositions{8};

    struct SelectingAbilityTarget {
      rts::AbilityInstanceIndex abilityIndex;
      bool afterEnqueue{false};
    };

    rts::SideId side;
    Camera camera;
    WorldView worldView;
    rts::AbilityPage abilityPage{0};
    std::array<std::optional<rts::Point>, MaxCameraPositions> cameraPositions_;
    std::optional<rts::Rectangle> selectionBox;
    std::optional<SelectingAbilityTarget> selectingAbilityTarget;
    bool minimapDrag{false};

    Player(rts::SideId s, Camera&& c) : side{s}, camera{std::move(c)} {}
    void update(const rts::World& w);
    std::optional<rts::Command> processInput(const rts::World& w, const InputEvent& event);

  private:
    using Clock = std::chrono::steady_clock;

    struct Event {
      InputEvent e{InputType::Unknown};
      Clock::time_point time;
    };

    enum class State { Default, BuildingPrototype, BuildTriggered };

    Event lastEvent_;
    int lastCycledBase_{-1};
    rts::Point selectionBoxStart_;
    State state_{State::Default};
    rts::AbilityInstanceIndex lastBuildAbilityIndex_;

    void cycleBase(const rts::World& w);
    rts::UnitIdList selectedSameType(const rts::World& w, rts::UnitId unit) const;
    rts::UnitIdList visibleSameType(const rts::World& w, rts::UnitId unit) const;
  };
}
