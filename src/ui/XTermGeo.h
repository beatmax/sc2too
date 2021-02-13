#pragma once

#include "ui/types.h"

namespace ui {
  class XTermGeo {
  public:
    void update();
    bool outdated();
    SubcharPoint toSubcharPoint(PixelPoint pp) const;
    const PixelRect& rect() const { return terminalRect_; }

  private:
    struct Calibration {
      PixelPoint base;
      PixelVector charSize;
      FVector factor;
    };

    bool calibrate();

    Calibration calibration_;
    PixelRect terminalRect_{};
  };
}
