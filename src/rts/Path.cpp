#include "rts/Path.h"

#include "rts/World.h"
#include "util/pathfinding.h"

#include <cstdlib>
#include <unordered_set>
#include <utility>

namespace rts {
  namespace {
    constexpr float DirD{1.0};
    constexpr float DiagD{1.41};

    float diagonalDistance(Point a, Point b) {
      auto dx{abs(a.x - b.x)};
      auto dy{abs(a.y - b.y)};
      return float(dx + dy) + (DiagD - 2.) * float(std::min(dx, dy));
    }

    const std::pair<Vector, float> NeighborVectors[]{
        {{-1, -1}, DiagD}, {{0, -1}, DirD},  {{1, -1}, DiagD}, {{-1, 0}, DirD},
        {{1, 0}, DirD},    {{-1, 1}, DiagD}, {{0, 1}, DirD},   {{1, 1}, DiagD}};

    struct Graph {
      using Id = Point;
      using Cost = float;
      using Path = rts::Path;

      using PointSet = std::unordered_set<Point>;

      constexpr static int MaxNeighbors{8};

      const Map& map;
      const PointSet adjacentObjectsPoints;

      Graph(const World& world, Point start)
        : map{world.map}, adjacentObjectsPoints(calcAdjacentObjectsPoints(world, start)) {}

      size_t size() const { return map.size(); }

      bool inBounds(Point p) const {
        return p.x >= 0 && p.x < map.maxX && p.y >= 0 && p.y < map.maxY;
      }

      int neighbors(Point p, std::pair<Point, float> result[]) const;

    private:
      PointSet calcAdjacentObjectsPoints(const World& world, Point start);
    };

    int Graph::neighbors(Point p, std::pair<Point, float> result[]) const {
      int n{0};
      for (auto [v, d] : NeighborVectors) {
        auto q{p + v};
        if (inBounds(q) && !hasBlocker(map.at(q)) &&
            adjacentObjectsPoints.find(q) == adjacentObjectsPoints.end()) {
          result[n++] = {q, d};
        }
      }
      return n;
    }

    auto Graph::calcAdjacentObjectsPoints(const World& world, Point start) -> PointSet {
      PointSet points;

      for (auto [v, d] : NeighborVectors) {
        auto np{start + v};
        if (inBounds(np) && !hasBlocker(world.map.at(np))) {
          if (auto object{world.objectPtrAt(np)}; object)
            forEachPoint(object->area, [&points](Point p) { points.insert(p); });
        }
      }

      return points;
    }
  }
}

std::pair<rts::Path, bool> rts::findPath(const World& world, Point start, Point goal) {
  using AStar = util::AStar<Graph>;

  auto object{world.objectPtrAt(goal)};
  auto goalArea{object ? boundingBox(object->area) : Rectangle{goal, {1, 1}}};
  auto isGoal{[goalArea](Point p) { return goalArea.contains(p); }};

  auto h{[goal](Point p) { return diagonalDistance(p, goal); }};

  AStar::StateMap state;
  Point closest{AStar::search(Graph{world, start}, start, isGoal, h, state)};

#ifdef MAP_DEBUG
  auto& m{const_cast<Map&>(world.map)};
  forEachPoint(Rectangle{{0, 0}, {m.maxX, m.maxY}}, [&](Point p) {
    m.at(p).debug.color = state.count(p) ? 4 : 0;
  });
#endif

  auto path{util::reconstructPath<Path>(state, start, closest)};
  return {std::move(path), isGoal(closest)};
}
