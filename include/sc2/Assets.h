#pragma once

#include "CellCreator.h"
#include "ui/Sprite.h"

namespace sc2 {
  class Assets {
  public:
    static void init();
    static const CellCreator& cellCreator() { return cellCreator_; }
    static const ::ui::Sprite& getSprite(const std::string& name);

  private:
    static CellCreator cellCreator_;
  };
}
