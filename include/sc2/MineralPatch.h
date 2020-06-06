#pragma once

#include "rts/ResourceField.h"
#include "ui.h"

namespace sc2 {
  class MineralPatch : public rts::ResourceFieldTpl<MineralPatch, ui::MineralPatch> {
    using Inherited = rts::ResourceFieldTpl<MineralPatch, ui::MineralPatch>;

  public:
    explicit MineralPatch(rts::Point p);
  };
}
