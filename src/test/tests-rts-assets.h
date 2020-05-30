#pragma once

#include "rts/Entity.h"

#include <string>

namespace test {
  extern const std::string map;

  class Simpleton : public rts::EntityTpl<Simpleton, rts::Ui> {
    using Inherited = rts::EntityTpl<Simpleton, rts::Ui>;

  public:
    explicit Simpleton(rts::Position p);
  };
}
