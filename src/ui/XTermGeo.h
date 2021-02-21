#pragma once

#include "ui/types.h"

namespace ui {
  class XTermGeo {
  public:
    void update();
    bool outdated();
    SubcharPoint toSubcharPoint(PixelPoint pp) const;
    const PixelRect& area() const { return calibration_.area; }

  private:
    struct Calibration {
      PixelRect area;
      PixelVector charSize;
      FVector factor;
    };

    bool calibrate();

    Calibration calibration_;
    PixelVector terminalSize_{};
  };
}
