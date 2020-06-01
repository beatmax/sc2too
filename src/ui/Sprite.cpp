#include "ui/Sprite.h"

#include "dim.h"
#include "util/fs.h"

#include <cassert>
#include <sstream>
#include <utility>

ui::Sprite::Sprite(const std::wstring& s) : Sprite{std::wistringstream{s}} {
}

ui::Sprite::Sprite(std::wistream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(const std::vector<std::wstring>& lines)
  : matrix(lines.size(), lines.empty() ? 0 : lines.front().size() / dim::cellWidth) {
  for (size_t y = 0; y < matrix.rows(); ++y) {
    const auto& line = lines[y];
    assert(line.size() == matrix.cols() * dim::cellWidth);
    for (size_t x = 0; x < matrix.cols(); ++x)
      matrix(y, x) = line.substr(x * dim::cellWidth, dim::cellWidth);
  }
}

rts::Rectangle ui::Sprite::area(rts::Point topLeft) const {
  return {topLeft, {rts::Coordinate(matrix.cols()), rts::Coordinate(matrix.rows())}};
}
