#pragma once

#include "World.h"
#include "rts/types.h"
#include "util/Clock.h"
#include "util/Timer.h"

#include <chrono>

namespace rts {
  template<typename Clock>
  class EngineBase {
  public:
    constexpr static GameSpeed DefaultGameSpeed{GameSpeedNormal};
    constexpr static Fps DefaultTargetFps{100};

    explicit EngineBase(World& world, GameSpeed speed = DefaultGameSpeed);

    template<typename C, typename FI, typename FO>
    void run(const C& controller, FI processInput, FO updateOutput);

    void advanceFrame();
    void advanceWorld();

    GameSpeed gameSpeed() const { return gameSpeed_; }
    void gameSpeed(GameSpeed speed);

    // to display game time, which can be accelerated / decelerated
    GameSpeed initialGameSpeed() const { return initialGameSpeed_; }

    Fps fps() const { return OneSecond / frameDuration_; }
    Fps targetFps() const { return targetFps_; }
    void targetFps(Fps fps);

  private:
    using Timer = util::Timer<Clock>;
    using Time = std::chrono::microseconds;

    constexpr static Time OneSecond = Time{1000000};

    void resetTimer();

    World& world_;
    Timer timer_;
    Fps targetFps_;
    Time targetFrameDuration_;
    Time frameDuration_;
    Time frameTime_;
    const GameSpeed initialGameSpeed_;
    GameSpeed gameSpeed_;
    Time timeUnitDuration_;
    GameTime refGameTime_;
  };

  template<typename Clock>
  template<typename C, typename FI, typename FO>
  void EngineBase<Clock>::run(const C& controller, FI processInput, FO updateOutput) {
    resetTimer();
    bool paused{false};

    while (!controller.quit()) {
      if (paused != controller.paused()) {
        paused = !paused;
        if (!paused)
          resetTimer();
      }

      if (!paused)
        advanceFrame();

      world_.update(processInput(world_));
      updateOutput(world_);
    }
  }

  class Engine : public EngineBase<util::RealClock> {
  public:
    using EngineBase<util::RealClock>::EngineBase;
  };
}
