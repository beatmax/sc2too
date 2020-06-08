#pragma once

#include "rts/Entity.h"
#include "ui.h"

namespace sc2 {
  class Nexus : public rts::EntityTpl<Nexus, ui::Nexus> {
    using Inherited = rts::EntityTpl<Nexus, ui::Nexus>;

  public:
    Nexus(rts::Point p, rts::SideCPtr sd);
  };
}
