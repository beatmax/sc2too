#include "rts/Map.h"

#include "rts/World.h"
#include "util/algorithm.h"
#include "util/fs.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <utility>

namespace rts {
  namespace {
    std::vector<std::string> expand(std::vector<std::string> lines) {
      std::string op;
      if (!lines.empty() && lines.back().rfind("@MIRROR", 0) == 0) {
        op = lines.back();
        lines.pop_back();
      }
      if (op == "@MIRROR-V" || op == "@MIRROR-VH") {
        auto mirror = lines;
        std::reverse(mirror.begin(), mirror.end());
        if (op == "@MIRROR-VH") {
          for (auto& line : mirror)
            std::reverse(line.begin(), line.end());
        }
        lines.insert(lines.end(), mirror.begin(), mirror.end());
      }
      return lines;
    }
  }
}

void rts::Map::load(World& w, const MapInitializer& init, const std::string& fpath) {
  std::ifstream f{fpath};
  if (!f)
    throw std::runtime_error{"cannot open map file: " + fpath};
  load(w, init, std::move(f));
}

void rts::Map::load(World& w, const MapInitializer& init, std::istream&& is) {
  load(w, init, expand(util::fs::readLines(is)));
}

void rts::Map::load(World& w, const MapInitializer& init, const std::vector<std::string>& lines) {
  const Coordinate cols{lines.empty() ? 0 : Coordinate(lines.front().size())};
  const Coordinate rows(lines.size());
  initCells({cols, rows});
  w.onMapCreated();
  for (Coordinate y = 0; y < rows; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == cols);
    for (Coordinate x = 0; x < cols; ++x) {
      Point p{x, y};
      if ((*this)[p].empty())
        std::visit([&w, p](auto id) { w.addForFree(id, p); }, init(w, p, line[x], lines));
    }
  }
  w.onMapLoaded();
}

bool rts::Map::isEmpty(const Rectangle& area) const {
  return util::allOf(area.points(), [this](Point p) { return (*this)[p].empty(); });
}
