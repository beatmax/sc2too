#pragma once

#include "ui/types.h"

#include <optional>

namespace ui {
  class XPixelTr {
  public:
    struct PointPair {
      ScreenPoint sp;
      PixelPoint pp;
    };

    std::optional<SubcharPoint> toSubcharPoint(PixelPoint pp) const;
    void update(const PointPair& p);
    void reset();

  private:
    struct Calibration {
      PixelPoint base;
      PixelVector charSize;
      FVector factor;
    };

    const PointPair* filter(const PointPair& p);
    void calibrate(const PointPair& target);
    bool checkCalibration(const PointPair& target);

    std::optional<Calibration> calibration_;
    PointPair prev_[2];
    size_t prevSize_{0};
  };
}
