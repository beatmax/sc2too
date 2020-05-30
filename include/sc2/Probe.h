#pragma once

#include "rts/Entity.h"

namespace sc2 {
  class Probe : public rts::EntityTpl<Probe> {
    using Inherited = rts::EntityTpl<Probe>;

  public:
    explicit Probe(rts::Position p);
  };
}
