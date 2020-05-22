#include "rts/Map.h"

#include "util/fs.h"

#include <cassert>
#include <sstream>

rts::Map::Map(const std::string& s) : Map{std::istringstream{s}} {
}

rts::Map::Map(std::istream&& is) : Map{util::fs::readLines(is)} {
}

rts::Map::Map(const std::vector<std::string>& lines)
  : maxX{lines.empty() ? 0 : lines.front().size()}, maxY{lines.size()}, cells_(maxX * maxY) {
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
