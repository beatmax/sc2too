#pragma once

#include <algorithm>
#include <limits>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace util {

  template<typename Graph>
  struct AStar {
    using Id = typename Graph::Id;
    using Cost = typename Graph::Cost;

    struct State {
      Id pred;
      Cost g;

      State() : g{std::numeric_limits<Cost>::max()} {}
      State(Id pred, Cost g) : pred{pred}, g{g} {}
    };

    using StateMap = std::unordered_map<Id, State>;

    struct OpenId {
      Id id;
      Cost f;

      OpenId(Id id, Cost f) : id{id}, f{f} {}
      bool operator>(const OpenId& other) const { return f > other.f; }
    };

    static auto makeOpenSet(const Graph& graph) {
      std::vector<OpenId> container;
      container.reserve(graph.size() / 2);
      return std::priority_queue<OpenId, std::vector<OpenId>, std::greater<OpenId>>{
          std::greater<OpenId>{}, std::move(container)};
    }

    template<typename IG, typename H>
    static Id search(const Graph& graph, Id start, IG isGoal, H h, StateMap& state);
  };

  template<typename Graph>
  template<typename IG, typename H>
  auto AStar<Graph>::search(const Graph& graph, Id start, IG isGoal, H h, StateMap& state) -> Id {
    auto openSet{makeOpenSet(graph)};

    Id closest{start};
    Cost hClosest{h(start)};

    state.emplace(start, State{start, 0});
    openSet.emplace(start, hClosest);

    while (!openSet.empty()) {
      auto current{openSet.top()};

      if (isGoal(current.id))
        return current.id;

      openSet.pop();
      auto gCurrent{state[current.id].g};

      std::pair<Id, Cost> neighbors[Graph::MaxNeighbors];
      int n{graph.neighbors(current.id, neighbors)};
      for (int i{0}; i < n; ++i) {
        const auto& neighbor{neighbors[i]};
        const Id& id{neighbor.first};
        auto d{neighbor.second};

        auto& s{state[id]};
        auto g{gCurrent + d};
        if (g < s.g) {
          s = {current.id, g};
          auto h_{h(id)};
          openSet.emplace(id, g + h_);
          if (hClosest > h_) {
            hClosest = h_;
            closest = id;
          }
        }
      }
    }

    return closest;
  }

  template<typename Path, typename StateMap, typename Id>
  Path reconstructPath(const StateMap& state, Id start, Id goal) {
    Path path;
    for (Id id{goal}; id != start; id = state.find(id)->second.pred)
      path.push_front(id);
    return path;
  }

}
