#include "ui/Sprite.h"

#include "SpriteMatrix.h"
#include "dim.h"
#include "util/fs.h"

#include <cassert>
#include <sstream>
#include <utility>

ui::Sprite::Sprite() = default;

ui::Sprite::Sprite(const std::wstring& s) : Sprite{std::wistringstream{s}} {
}

ui::Sprite::Sprite(std::wistream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(const std::vector<std::wstring>& lines)
  : matrix_{
        std::make_unique<SpriteMatrix>(lines.size(), lines.empty() ? 0 : lines.front().size())} {
  for (int y = 0; y < matrix_->rows(); ++y) {
    const auto& line = lines[y];
    assert(line.size() == size_t(matrix_->cols()));
    for (int x = 0; x < matrix_->cols(); ++x) {
      wchar_t wch[]{line[x], L'\0'};
      setcchar(&(*matrix_)(y, x), wch, 0, 2, nullptr);
    }
  }
}

ui::Sprite::~Sprite() = default;

rts::Rectangle ui::Sprite::area(rts::Point topLeft) const {
  return {topLeft,
          {rts::Coordinate((matrix_->cols() + 1) / (dim::cellWidth + 1)),
           rts::Coordinate((matrix_->rows() + 1) / (dim::cellHeight + 1))}};
}
