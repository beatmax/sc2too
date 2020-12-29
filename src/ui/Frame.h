#pragma once

#include "ui/types.h"
#include "util/Matrix.h"

namespace ui {
  class Frame : public util::Matrix<cchar_t, int> {
  public:
    using util::Matrix<cchar_t, int>::Matrix;

    Color defaultBg() const { return defaultBg_; }
    void setDefaultBg(Color c) { defaultBg_ = c; }

  private:
    Color defaultBg_{Color::Default};
  };
}
