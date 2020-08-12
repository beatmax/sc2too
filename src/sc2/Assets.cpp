#include "sc2/Assets.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace fs = std::filesystem;

namespace {
  std::map<std::string, ::ui::Sprite> sprites;
  std::map<std::string, ::ui::Icon> icons;
  const ::ui::Sprite spriteError{L"???\n"};
  const ::ui::Icon iconError{L"???\n"};
}

void sc2::Assets::init() {
  for (const auto& p : fs::directory_iterator("data/sprites"))
    sprites.emplace(p.path().stem(), std::wifstream(p.path()));
  for (const auto& p : fs::directory_iterator("data/icons"))
    icons.emplace(p.path().stem(), std::wifstream(p.path()));
}

const ui::Sprite& sc2::Assets::getSprite(const std::string& name) {
  auto it = sprites.find(name);
  return (it != sprites.end()) ? it->second : spriteError;
}

const ui::Icon& sc2::Assets::getIcon(const std::string& name) {
  auto it = icons.find(name);
  return (it != icons.end()) ? it->second : iconError;
}

sc2::MapInitializer sc2::Assets::mapInitializer_;
