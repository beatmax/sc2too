#pragma once

#include "ui/types.h"

#include <chrono>
#include <optional>

namespace ui {
  class XPixelTr {
  public:
    using TimePoint = std::chrono::steady_clock::time_point;

    struct Sample {
      TimePoint tp;
      ScreenPoint sp;
      PixelPoint pp;
    };

    std::optional<SubcharPoint> toSubcharPoint(PixelPoint pp) const;
    void update(const Sample& p);
    void reset();

  private:
    struct Calibration {
      PixelPoint base;
      PixelVector charSize;
      FVector factor;
    };

    std::optional<Sample> filter(const Sample& p);
    bool stableDirection(PixelPoint pp);
    void calibrate(const Sample& target);
    bool checkCalibration(const Sample& target);

    std::optional<Calibration> calibration_;
    std::optional<Sample> candidate_;
    PixelPoint ppPrev_;
    PixelVector ppDirection_;
  };
}
