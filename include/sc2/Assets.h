#pragma once

#include "MapInitializer.h"
#include "ui/Sprite.h"

namespace sc2 {
  class Assets {
  public:
    static void init();
    static const MapInitializer& mapInitializer() { return mapInitializer_; }
    static const ::ui::Sprite& getSprite(const std::string& name);

  private:
    static MapInitializer mapInitializer_;
  };
}
