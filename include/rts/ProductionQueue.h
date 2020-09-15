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

    bool add(World& w, UnitTypeId type);
    bool startProduction(World& w, bool retrying);
    bool finishProduction(World& w, const Unit& parent);

    SideId side() const { return side_; }
    size_t size() const { return queue_.size(); }
    bool empty() const { return queue_.empty(); }
    UnitTypeId top() const { return queue_.front(); }
    UnitTypeId type(size_t index) const { return queue_[index]; }
    GameTime buildTime(const World& w) const;

    void setRallyPoint(Point p) { rallyPoint_ = p; }
    const std::optional<Point>& rallyPoint() const { return rallyPoint_; }

  private:
    void create(World& w, UnitTypeId type, Point p);

    util::CircularBuffer<UnitTypeId, MaxProductionQueueSize> queue_;
    SideId side_;
    ResourceBagArray resources_;
    std::optional<Point> rallyPoint_;
  };
}
