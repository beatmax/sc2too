#pragma once

#include "rts/Entity.h"
#include "ui.h"

namespace sc2 {
  class Probe : public rts::EntityTpl<Probe, ui::Probe> {
    using Inherited = rts::EntityTpl<Probe, ui::Probe>;

  public:
    explicit Probe(rts::Position p);
  };
}
