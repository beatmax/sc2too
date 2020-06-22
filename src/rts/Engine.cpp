#include "rts/Engine.h"

template<typename Clock>
rts::EngineBase<Clock>::EngineBase(World& world)
  : world_{world}, initialGameSpeed_{DefaultGameSpeed} {
  gameSpeed(initialGameSpeed_);
  targetFps(DefaultTargetFps);
  frameDuration_ = targetFrameDuration_;
}

template<typename Clock>
void rts::EngineBase<Clock>::advanceFrame() {
  Time prevFrameTime{frameTime_};
  Time now{timer_.elapsed()};

  frameTime_ += targetFrameDuration_;
  if (frameTime_ < now)
    frameTime_ = now;
  frameDuration_ = frameTime_ - prevFrameTime;

  const GameTime nextFrameGameTime = refGameTime_ + frameTime_ / timeUnitDuration_;

  while (world_.time < nextFrameGameTime)
    advanceWorld();

  timer_.sleepUntil(frameTime_);
}

template<typename Clock>
void rts::EngineBase<Clock>::advanceWorld() {
  ++world_.time;

  for (const auto& e : world_.entities) {
    if (e)
      world_.update(Entity::step(world_, e));
  }
}

template<typename Clock>
void rts::EngineBase<Clock>::gameSpeed(GameSpeed speed) {
  gameSpeed_ = speed;
  timeUnitDuration_ = OneSecond / gameSpeed_;
  resetTimer();
}

template<typename Clock>
void rts::EngineBase<Clock>::targetFps(Fps fps) {
  targetFps_ = fps;
  targetFrameDuration_ = OneSecond / targetFps_;
}

template<typename Clock>
void rts::EngineBase<Clock>::resetTimer() {
  timer_.reset();
  frameTime_ = Time{};
  refGameTime_ = world_.time;
}

template class rts::EngineBase<util::RealClock>;
template class rts::EngineBase<util::FakeClock>;
