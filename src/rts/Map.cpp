#include "rts/Map.h"

#include "util/fs.h"

#include <cassert>
#include <sstream>

rts::Map::Map(const std::string& s) : Map{std::istringstream{s}} {
}

rts::Map::Map(std::istream&& is) : Map{util::fs::readLines(is)} {
}

rts::Map::Map(const std::vector<std::string>& lines)
  : maxX{lines.empty() ? 0 : Coordinate(lines.front().size())},
    maxY{Coordinate(lines.size())},
    cells_(maxY, maxX) {
  for (Coordinate y = 0; y < maxY; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == maxX);
    for (Coordinate x = 0; x < maxX; ++x) {
      switch (line[x]) {
        case 'r':
          set({x, y}, Blocker::Rock);
          break;
      }
    }
  }
}
