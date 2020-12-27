#include "rts/Path.h"

#include "rts/World.h"
#include "util/Overloaded.h"
#include "util/pathfinding.h"

#include <cassert>
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

      constexpr static int MaxNeighbors{8};

      const World& w;
      const ExcludedPointSet& excludedPoints;

      Graph(const World& w, Point start, const ExcludedPointSet& excl)
        : w{w}, excludedPoints{excl} {}

      size_t size() const { return w.map.cellCount(); }
      bool contains(Point p) const;
      int neighbors(Point p, std::pair<Point, float> result[]) const;
    };

    bool Graph::contains(Point p) const {
      if (!w.inBounds(p) || excludedPoints.find(p) != excludedPoints.end())
        return false;
      const auto& cell{w.map[p]};
      return cell.empty() || (cell.contains(Cell::Unit) && !w[cell.unitId()].isStructure(w));
    }

    int Graph::neighbors(Point p, std::pair<Point, float> result[]) const {
      int n{0};
      for (auto [v, d] : NeighborVectors) {
        auto q{p + v};
        if (contains(q)) {
          result[n++] = {q, d};
        }
      }
      return n;
    }
  }
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, Point goal, const ExcludedPointSet& excl) {
  auto object{w.object(goal)};
  auto goalArea{object ? boundingBox(object->area) : Rectangle{goal, {1, 1}}};
  return findPath(w, start, goal, goalArea, excl);
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, const Rectangle& goalArea, const ExcludedPointSet& excl) {
  return findPath(w, start, goalArea.center(), goalArea, excl);
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w,
    Point start,
    Point goal,
    const Rectangle& goalArea,
    const ExcludedPointSet& excl) {
  using AStar = util::AStar<Graph>;

  auto isGoal{[goalArea](Point p) { return goalArea.contains(p); }};
  auto h{[goal](Point p) { return diagonalDistance(p, goal); }};

  AStar::StateMap state;
  Point closest{AStar::search(Graph{w, start, excl}, start, isGoal, h, state)};

#ifdef MAP_DEBUG
  auto& m{const_cast<Map&>(w.map)};
  for (Point p : m.area().points())
    m[p].debug.highlight = state.count(p);
#endif

  auto path{util::reconstructPath<Path>(state, start, closest)};
  return {std::move(path), isGoal(closest)};
}

std::pair<rts::Path, bool> rts::findPathToTarget(
    const World& w, Point start, const AbilityTarget& target, const ExcludedPointSet& excl) {
  return std::visit(
      util::Overloaded{
          [&](Point p) { return findPath(w, start, p, excl); },
          [&](auto id) { return findPath(w, start, boundingBox(w[id].area), excl); }},
      target);
}

bool rts::dodge(const World& w, Point pos, Path& path) {
  if (path.size() < 2)
    return false;

  auto v{path[1] - pos};
  if (v.x == 0 || v.y == 0) {
    Vector opts[2];
    if (v.x == 0) {
      auto y{v.y / 2};
      opts[0] = {1, y};
      opts[1] = {-1, y};
    }
    else {
      auto x{v.x / 2};
      opts[0] = {x, 1};
      opts[1] = {x, -1};
    }

    for (int i{0}; i < 2; ++i) {
      auto p{pos + opts[i]};
      if (w.inBounds(p) && w[p].empty()) {
        path.front() = p;
        return true;
      }
    }
  }
  else {
    Vector vabs{labs(v.x), labs(v.y)};
    if (vabs.x == 2 && vabs.y == 2) {
      Vector opts[2][2]{{{v.x / 2, 0}, {v.x, v.y / 2}}, {{0, v.y / 2}, {v.x / 2, v.y}}};
      for (int i{0}; i < 2; ++i) {
        auto p1{pos + opts[i][0]};
        auto p2{pos + opts[i][1]};
        assert(w.inBounds(p1) && w.inBounds(p2));
        if (w[p1].empty() && w[p2].empty()) {
          path.front() = p2;
          path.push_front(p1);
          return true;
        }
      }
    }
    else if (vabs.x == 2) {
      assert(vabs.y == 1);
      auto p{pos + Vector{v.x / 2, (path.front().y == pos.y) ? v.y : 0}};
      assert(w.inBounds(p));
      if (w[p].empty()) {
        path.front() = p;
        return true;
      }
    }
    else if (vabs.y == 2) {
      assert(vabs.x == 1);
      auto p{pos + Vector{(path.front().x == pos.x) ? v.x : 0, v.y / 2}};
      assert(w.inBounds(p));
      if (w[p].empty()) {
        path.front() = p;
        return true;
      }
    }
  }

  return false;
}
