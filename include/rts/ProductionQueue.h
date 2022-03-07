#pragma once

#include "Resource.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"
#include "util/CircularBuffer.h"

#include <optional>

namespace rts {
  class ProductionQueue {
  public:
    explicit ProductionQueue(SideId side) : side_{side} {};
    void onDestroy(World& w);

    bool add(World& w, ProducibleId product);
    bool startProduction(World& w, bool retrying);
    bool finishProduction(World& w, const Unit& parent);

    SideId side() const { return side_; }
    size_t size() const { return queue_.size(); }
    bool empty() const { return queue_.empty(); }
    ProducibleId top() const { return queue_.front(); }
    ProducibleId product(size_t index) const { return queue_[index]; }
    GameTime produceTime(const World& w) const;

    void setRallyPoint(Point p) { rallyPoint_ = p; }
    const std::optional<Point>& rallyPoint() const { return rallyPoint_; }

    void boost(const World& w, Percent speedUp, GameTime duration);
    bool boosted(const World& w) const;
    GameTime workPerCycle(const World& w) const;

  private:
    void create(World& w, UnitTypeId type, Point p);

    util::CircularBuffer<ProducibleId, MaxProductionQueueSize> queue_;
    SideId side_;
    ResourceBagArray resources_;
    std::optional<Point> rallyPoint_;
    GameTime boostUntil_{};
    GameTime boostWorkPerCycle_{};
  };
}
