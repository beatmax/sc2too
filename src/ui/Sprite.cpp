#include "ui/Sprite.h"

#include "util/fs.h"

#include <cassert>
#include <sstream>
#include <utility>

ui::Sprite::Sprite(const std::string& s) : Sprite{std::istringstream{s}} {
}

ui::Sprite::Sprite(std::istream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(const std::vector<std::string>& lines)
  : matrix(lines.size(), lines.empty() ? 0 : lines.front().size()) {
  for (size_t y = 0; y < matrix.rows(); ++y) {
    const auto& line = lines[y];
    assert(line.size() == matrix.cols());
    for (size_t x = 0; x < matrix.cols(); ++x) {
      chtype c = line[x];
      if (c == '#')  // TODO use utf-8 to represent graphic characters
        c = ACS_CKBOARD;
      matrix(y, x) = c;
    }
  }
}
