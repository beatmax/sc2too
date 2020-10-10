#include "rts/Path.h"

#include "rts/World.h"
#include "util/pathfinding.h"

#include <unordered_set>
#include <utility>

namespace rts {
  namespace {
    using util::geo::DiagD;
    using util::geo::diagonalDistance;
    using util::geo::DirD;

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

      Graph(const World& w, Point start)
        : map{w.map}, adjacentObjectsPoints(calcAdjacentObjectsPoints(w, start)) {}

      size_t size() const { return map.cellCount(); }
      bool inBounds(Point p) const { return map.area().contains(p); }
      int neighbors(Point p, std::pair<Point, float> result[]) const;

    private:
      PointSet calcAdjacentObjectsPoints(const World& w, Point start);
    };

    int Graph::neighbors(Point p, std::pair<Point, float> result[]) const {
      int n{0};
      for (auto [v, d] : NeighborVectors) {
        auto q{p + v};
        if (inBounds(q) && !map[q].contains(Cell::Blocker) &&
            adjacentObjectsPoints.find(q) == adjacentObjectsPoints.end()) {
          result[n++] = {q, d};
        }
      }
      return n;
    }

    auto Graph::calcAdjacentObjectsPoints(const World& w, Point start) -> PointSet {
      PointSet points;

      for (auto [v, d] : NeighborVectors) {
        auto np{start + v};
        if (inBounds(np) && !w[np].contains(Cell::Blocker)) {
          if (auto object{w.object(np)}) {
            for (Point p : object->area.points())
              points.insert(p);
          }
        }
      }

      return points;
    }
  }
}

std::pair<rts::Path, bool> rts::findPath(const World& w, Point start, Point goal) {
  auto object{w.object(goal)};
  auto goalArea{object ? boundingBox(object->area) : Rectangle{goal, {1, 1}}};
  return findPath(w, start, goal, goalArea);
}

std::pair<rts::Path, bool> rts::findPath(const World& w, Point start, const Rectangle& goalArea) {
  return findPath(w, start, goalArea.center(), goalArea);
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, Point goal, const Rectangle& goalArea) {
  using AStar = util::AStar<Graph>;

  auto isGoal{[goalArea](Point p) { return goalArea.contains(p); }};
  auto h{[goal](Point p) { return diagonalDistance(p, goal); }};

  AStar::StateMap state;
  Point closest{AStar::search(Graph{w, start}, start, isGoal, h, state)};

#ifdef MAP_DEBUG
  auto& m{const_cast<Map&>(w.map)};
  for (Point p : m.area().points())
    m[p].debug.highlight = state.count(p);
#endif

  auto path{util::reconstructPath<Path>(state, start, closest)};
  return {std::move(path), isGoal(closest)};
}

std::pair<rts::Path, bool> rts::findPath(const World& w, Point start, UnitId unit) {
  return findPath(w, start, boundingBox(w[unit].area));
}

std::pair<rts::Path, bool> rts::findPathToTarget(
    const World& w, Point start, const AbilityTarget& target) {
  return std::visit([&](const auto& t) { return findPath(w, start, t); }, target);
}
