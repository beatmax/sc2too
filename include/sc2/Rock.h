#pragma once

#include "rts/Blocker.h"
#include "ui.h"

namespace sc2 {
  class Rock : public rts::BlockerTpl<Rock, ui::Rock> {
    using Inherited = rts::BlockerTpl<Rock, ui::Rock>;

  public:
    explicit Rock(rts::Point p);
  };
}
