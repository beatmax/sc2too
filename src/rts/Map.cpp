#include "rts/Map.h"

#include "util/fs.h"

#include <algorithm>
#include <cassert>

namespace rts {
  namespace {
    std::vector<std::string> expand(std::vector<std::string> lines) {
      if (!lines.empty() && lines.back() == "@MIRROR-VH") {
        lines.pop_back();
        auto mirror = lines;
        std::reverse(mirror.begin(), mirror.end());
        for (auto& line : mirror)
          std::reverse(line.begin(), line.end());
        lines.insert(lines.end(), mirror.begin(), mirror.end());
      }
      return lines;
    }
  }
}

void rts::Map::load(World& w, const MapInitializer& init, std::istream&& is) {
  load(w, init, expand(util::fs::readLines(is)));
}

void rts::Map::load(World& w, const MapInitializer& init, const std::vector<std::string>& lines) {
  const Coordinate cols{lines.empty() ? 0 : Coordinate(lines.front().size())};
  const Coordinate rows(lines.size());
  cells_ = CellMatrix{rows, cols};
  for (Coordinate y = 0; y < rows; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == cols);
    for (Coordinate x = 0; x < cols; ++x) {
      Point p{x, y};
      if ((*this)[p].empty())
        init(w, p, line[x]);
    }
  }
}
