#include "rts/Path.h"

#include "rts/World.h"
#include "util/Overloaded.h"
#include "util/algorithm.h"
#include "util/pathfinding.h"

#include <cassert>
#include <queue>
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

      Graph(const World& w, const ExcludedPointSet& excl) : w{w}, excludedPoints{excl} {}

      size_t size() const { return w.map.cellCount(); }
      bool contains(Point p) const;
      int neighbors(Point p, std::pair<Point, float> result[]) const;
    };

    bool Graph::contains(Point p) const {
      if (!w.inBounds(p) || excludedPoints.find(p) != excludedPoints.end())
        return false;
      const auto& cell{w[p]};
      return cell.empty() || (cell.contains(Cell::Unit) && !w[cell.unitId()].isStructure(w));
    }

    int Graph::neighbors(Point p, std::pair<Point, float> result[]) const {
      int n{0};
      for (auto [v, d] : NeighborVectors) {
        auto q{p + v};
        if (contains(q))
          result[n++] = {q, d};
      }
      return n;
    }

    void floodFill(World& w, const Graph& graph, Point start, MapSegmentId segment) {
      std::queue<Point> queue;
      queue.push(start);
      w.map[start].segment = segment;
      while (!queue.empty()) {
        auto p{queue.front()};
        queue.pop();
        for (const auto& v : NeighborVectors) {
          auto q{p + v.first};
          if (graph.contains(q) && w[q].segment != segment) {
            queue.push(q);
            w.map[q].segment = segment;
          }
        }
      }
    }
  }
}

void rts::initMapSegments(World& w) {
  const ExcludedPointSet eps;
  Graph graph{w, eps};
  MapSegmentId segment{0};
  for (Point p : w.map.area().points())
    w[p].segment = 0;
  for (Point p : w.map.area().points()) {
    if (graph.contains(p) && !w[p].segment)
      floodFill(w, graph, p, ++segment);
  }
}

rts::Point rts::closestConnectedPoint(const World& w, Point p, Point goal) {
  Point closest;
  float closestDistance{std::numeric_limits<float>::infinity()};
  auto segment{w[p].segment};
  Rectangle area{goal, {1, 1}};
  for (float radius{0.}; radius < closestDistance; radius += 1., area = boundingBox(area)) {
    for (auto q : area.outerPoints()) {
      if (w.inBounds(q) && w[q].segment == segment) {
        if (float d{diagonalDistance(q, goal)}; d < closestDistance) {
          closestDistance = d;
          closest = q;
        }
      }
    }
  }
  return closest;
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, Point goal, const ExcludedPointSet& excl) {
  auto object{w.object(goal)};
  auto goalArea{
      object ? intersection(w.map.area(), boundingBox(object->area)) : Rectangle{goal, {1, 1}}};
  return findPath(w, start, goal, goalArea, excl);
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, const Rectangle& goalArea, const ExcludedPointSet& excl) {
  return findPath(w, start, goalArea.center(), goalArea, excl);
}

std::pair<rts::Path, bool> rts::findPath(
    const World& w, Point start, Point goal, Rectangle goalArea, const ExcludedPointSet& excl) {
  using AStar = util::AStar<Graph>;

  assert(goalArea.contains(goal));
  assert(w.map.area().contains(goalArea));

  const bool reachable{util::anyOf(
      goalArea.outerPoints(),
      [&w, segment{w[start].segment}](Point p) { return w[p].segment == segment; })};
  if (!reachable) {
    goal = closestConnectedPoint(w, start, goal);
    goalArea = Rectangle{goal, {1, 1}};
  }

  auto isGoal{[goalArea](Point p) { return goalArea.contains(p); }};
  auto h{[goal](Point p) { return diagonalDistance(p, goal); }};

  AStar::StateMap state;
  Point closest{AStar::search(Graph{w, excl}, start, isGoal, h, state)};

#ifdef MAP_DEBUG
  auto& m{const_cast<Map&>(w.map)};
  for (Point p : m.area().points())
    m[p].debug.highlight = state.count(p);
#endif

  auto path{util::reconstructPath<Path>(state, start, closest)};
  return {std::move(path), reachable && isGoal(closest)};
}

std::pair<rts::Path, bool> rts::findPathToTarget(
    const World& w, Point start, const AbilityTarget& target, const ExcludedPointSet& excl) {
  return std::visit(
      util::Overloaded{
          [](std::monostate) { return std::make_pair(rts::Path{}, false); },
          [&](Point p) { return findPath(w, start, p, excl); },
          [&](auto id) {
            return findPath(w, start, intersection(w.map.area(), boundingBox(w[id].area)), excl);
          }},
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
    Vector vabs{abs(v.x), abs(v.y)};
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
