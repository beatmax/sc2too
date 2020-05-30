#include "ui/Sprite.h"

#include "util/fs.h"

#include <cassert>
#include <sstream>
#include <utility>

namespace ui::sprite {
  Sprite rock;
  Sprite error{"???\n"};
}

ui::Sprite::Sprite(const std::string& s) : Sprite{std::istringstream{s}} {
}

ui::Sprite::Sprite(std::istream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(const std::vector<std::string>& lines)
  : matrix(lines.size(), lines.empty() ? 0 : lines.front().size()) {
  for (size_t y = 0; y < matrix.rows(); ++y) {
    const auto& line = lines[y];
    assert(line.size() == matrix.cols());
    for (size_t x = 0; x < matrix.cols(); ++x)
      matrix(y, x) = line[x];
  }
}

void ui::loadSprites() {
  sprite::rock = Sprite{ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD};
}

const ui::Sprite& ui::getSprite(rts::Blocker blocker) {
  switch (blocker) {
    case rts::Blocker::Rock:
      return sprite::rock;
  }
  return sprite::error;
}
