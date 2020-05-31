#include "sc2/Assets.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace fs = std::filesystem;

namespace {
  std::map<std::string, ::ui::Sprite> sprites;
  const ::ui::Sprite spriteError{"???\n"};
}

void sc2::Assets::init() {
  for (const auto& p : fs::directory_iterator("data/sprites"))
    sprites.emplace(p.path().stem(), std::ifstream(p.path()));
}

const ui::Sprite& sc2::Assets::getSprite(const std::string& name) {
  auto it = sprites.find(name);
  if (it != sprites.end())
    return it->second;
  else
    return spriteError;
}

sc2::CellCreator sc2::Assets::cellCreator_;
