#include "ui/Sprite.h"

#include "util/fs.h"

#include <cassert>
#include <sstream>
#include <utility>

ui::Sprite::Sprite(const std::wstring& s) : Sprite{std::wistringstream{s}} {
}

ui::Sprite::Sprite(std::wistream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(const std::vector<std::wstring>& lines)
  : matrix(
        (lines.size() + 1) / 2,
        lines.empty() ? 0 : (lines.front().size() + 1) / (dim::cellWidth + 1)) {
  for (size_t y = 0; y < matrix.rows(); ++y) {
    const size_t cellHeight = dim::cellHeight + ((y < matrix.rows() - 1) ? 1 : 0);
    for (size_t cellY = 0; cellY < cellHeight; ++cellY) {
      const auto& line = lines[y * (dim::cellHeight + 1) + cellY];
      assert(line.size() == matrix.cols() * (dim::cellWidth + 1) - 1);
      for (size_t x = 0; x < matrix.cols(); ++x)
        matrix(y, x)[cellY] = line.substr(x * (dim::cellWidth + 1), dim::cellWidth + 1);
    }
  }
}

rts::Rectangle ui::Sprite::area(rts::Point topLeft) const {
  return {topLeft, {rts::Coordinate(matrix.cols()), rts::Coordinate(matrix.rows())}};
}
