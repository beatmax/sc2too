#include "rts/Map.h"

#include "rts/Entity.h"
#include "util/fs.h"

#include <cassert>

rts::Map::Map(const World& world, const CellCreator& creator, std::istream&& is)
  : Map{world, creator, util::fs::readLines(is)} {
}

rts::Map::Map(const World& world, const CellCreator& creator, const std::vector<std::string>& lines)
  : maxX{lines.empty() ? 0 : Coordinate(lines.front().size())},
    maxY{Coordinate(lines.size())},
    cells_(maxY, maxX) {
  for (Coordinate y = 0; y < maxY; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == maxX);
    for (Coordinate x = 0; x < maxX; ++x) {
      Point p{x, y};
      if (isFree(at(p))) {
        Cell cell{creator(world, p, line[x])};
        if (hasWorldObject(cell))
          set(getWorldObject(cell).area, cell);
      }
    }
  }
}
