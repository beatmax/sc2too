#include "rts/Engine.h"

#include <thread>

void rts::Engine::advanceFrame() {
  std::this_thread::sleep_until(nextFrameTime_);
  nextFrameTime_ =
      std::chrono::steady_clock::now() + std::chrono::microseconds{1000000 / targetFps_};
}
