#include "rts/Map.h"

#include "rts/Entity.h"
#include "rts/World.h"
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

    template<typename Id>
    WorldObject* worldObjectPtr(World& world, Id id) {
      return &world[id];
    }

    WorldObject* worldObjectPtr(World& world, Map::Free) { return nullptr; }
  }
}

void rts::Map::load(World& world, const MapInitializer& init, std::istream&& is) {
  load(world, init, expand(util::fs::readLines(is)));
}

void rts::Map::load(
    World& world, const MapInitializer& init, const std::vector<std::string>& lines) {
  const Coordinate cols{lines.empty() ? 0 : Coordinate(lines.front().size())};
  const Coordinate rows(lines.size());
  cells_ = CellMatrix{rows, cols};
  for (Coordinate y = 0; y < rows; ++y) {
    const auto& line = lines[y];
    assert(Coordinate(line.size()) == cols);
    for (Coordinate x = 0; x < cols; ++x) {
      Point p{x, y};
      if (isFree(at(p)))
        init(world, p, line[x]);
    }
  }
}

rts::WorldObject& rts::getObject(World& world, Map::Cell& c) {
  return std::visit([&world](auto id) -> rts::WorldObject& { return world[id]; }, c.content);
}

const rts::WorldObject& rts::getObject(const World& world, const Map::Cell& c) {
  return getObject(const_cast<World&>(world), const_cast<Map::Cell&>(c));
}

rts::WorldObject* rts::getObjectPtr(World& world, Map::Cell& c) {
  return std::visit([&world](auto id) { return worldObjectPtr(world, id); }, c.content);
}

const rts::WorldObject* rts::getObjectPtr(const World& world, const Map::Cell& c) {
  return getObjectPtr(const_cast<World&>(world), const_cast<Map::Cell&>(c));
}
