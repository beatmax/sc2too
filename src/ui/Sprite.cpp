#include "ui/Sprite.h"

#include "SpriteMatrix.h"
#include "dim.h"
#include "graph.h"
#include "util/fs.h"

#include <cassert>
#include <cwchar>
#include <map>
#include <sstream>
#include <utility>

namespace {
  using LineColors = std::vector<int>;
  using Parts = std::pair<std::vector<std::wstring>, std::vector<LineColors>>;

  constexpr int defaultColor = 15;

  Parts parse(std::vector<std::wstring>&& lines) {
    Parts result;

    auto& chars{result.first};
    auto& colors{result.second};

    auto it = lines.begin();
    for (; it < lines.end() && !it->empty(); ++it)
      chars.push_back(std::move(*it));

    if (it != lines.end())
      ++it;

    std::vector<std::wstring> colorLines;
    if (it < lines.end() && *it == L"=") {
      colorLines = chars;
      ++it;
      assert(it == lines.end() || it->empty());
    }
    else {
      for (; it < lines.end() && !it->empty(); ++it)
        colorLines.push_back(std::move(*it));
    }

    if (it != lines.end())
      ++it;

    std::map<wchar_t, int> colorMap;
    for (int n = 1; n <= 9; ++n)
      colorMap[L'0' + n] = n;
    for (; it < lines.end(); ++it) {
      const std::wstring& mapLine{*it};
      assert(mapLine.size() > 2 && mapLine[1] == L':');
      colorMap[mapLine[0]] = std::wcstol(mapLine.c_str() + 2, nullptr, 10);
    }

    for (it = colorLines.begin(); it < colorLines.end(); ++it) {
      LineColors lineColors;
      lineColors.reserve(it->size());
      for (auto c : *it) {
        auto itMap = colorMap.find(c);
        lineColors.push_back(itMap != colorMap.end() ? itMap->second : defaultColor);
      }
      colors.push_back(std::move(lineColors));
    }

    return result;
  }
}

int ui::detail::defaultDefaultColor() {
  return graph::white();
}

ui::Sprite::Sprite() = default;

ui::Sprite::Sprite(const std::wstring& s) : Sprite{std::wistringstream{s}} {
}

ui::Sprite::Sprite(std::wistream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(std::vector<std::wstring>&& lines) {
  auto [chars, colors] = parse(std::move(lines));
  matrix_ = std::make_unique<SpriteMatrix>(chars.size(), chars.empty() ? 0 : chars.front().size());
  for (int y = 0; y < matrix_->rows(); ++y) {
    const auto& line = chars[y];
    assert(line.size() == size_t(matrix_->cols()));
    for (int x = 0; x < matrix_->cols(); ++x) {
      wchar_t wch[]{line[x], L'\0'};
      const int color =
          (y < int(colors.size()) && x < int(colors[y].size())) ? colors[y][x] : defaultColor;
      setcchar(&(*matrix_)(y, x), wch, 0, color, nullptr);
    }
  }
}

ui::Sprite::~Sprite() = default;

rts::Rectangle ui::Sprite::area(rts::Point topLeft) const {
  return {
      topLeft,
      {rts::Coordinate((matrix_->cols() + 1) / (dim::cellWidth + 1)),
       rts::Coordinate((matrix_->rows() + 1) / (dim::cellHeight + 1))}};
}
