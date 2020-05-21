#include "rts/Map.h"

#include "util/fs.h"

#include <cassert>

void rts::Map::load(std::istream& is) {
  auto lines = util::fs::readLines(is);
  maxX = lines.empty() ? 0 : lines.front().size();
  maxY = lines.size();
  cells_ = std::vector<Cell>(maxX * maxY);
  for (size_t y = 0; y < maxY; ++y) {
    const auto& line = lines[y];
    assert(line.size() == maxX);
    for (size_t x = 0; x < maxX; ++x) {
      switch (line[x]) {
        case 'r':
          set({x, y}, Blocker::Rock);
          break;
      }
    }
  }
}
