#include "rts/Map.h"

#include "rts/Entity.h"
#include "util/fs.h"

#include <cassert>
#include <sstream>

rts::Map::Map(const CellCreator& creator, const std::string& s)
  : Map{creator, std::istringstream{s}} {
}

rts::Map::Map(const CellCreator& creator, std::istream&& is)
  : Map{creator, util::fs::readLines(is)} {
}

rts::Map::Map(const CellCreator& creator, const std::vector<std::string>& lines)
  : maxX{lines.empty() ? 0 : Coordinate(lines.front().size())},
    maxY{Coordinate(lines.size())},
    cells_(maxY, maxX) {
  for (Coordinate y = 0; y < maxY; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == maxX);
    for (Coordinate x = 0; x < maxX; ++x) {
      Point p{x, y};
      if (isFree(at(p))) {
        Cell cell{creator(line[x], p)};
        if (hasWorldObject(cell))
          set(getWorldObject(cell).area, cell);
      }
    }
  }
}
