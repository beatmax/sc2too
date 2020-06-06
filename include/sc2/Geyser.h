#pragma once

#include "rts/ResourceField.h"
#include "ui.h"

namespace sc2 {
  class Geyser : public rts::ResourceFieldTpl<Geyser, ui::Geyser> {
    using Inherited = rts::ResourceFieldTpl<Geyser, ui::Geyser>;

  public:
    explicit Geyser(rts::Point p);
  };
}
