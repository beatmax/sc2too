#pragma once

#include "types.h"

#include <memory>
#include <utility>

namespace rts {
  class Blocker {
  public:
    explicit Blocker(UiUPtr ui) : ui_{std::move(ui)} {}
    const Ui& ui() const { return *ui_; }

  private:
    UiUPtr ui_;
  };
}
