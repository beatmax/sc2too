#pragma once

#include "Resource.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"
#include "util/CircularBuffer.h"

namespace rts {
  class ProductionQueue {
  public:
    explicit ProductionQueue(SideId side) : side_{side} {};

    bool add(World& w, EntityTypeId type);
    bool startProduction(World& w);
    bool finishProduction(World& w, const Entity& parent);

    SideId side() const { return side_; }
    size_t size() const { return queue_.size(); }
    bool empty() const { return queue_.empty(); }
    EntityTypeId top() const { return queue_.front(); }
    EntityTypeId type(size_t index) const { return queue_[index]; }
    GameTime buildTime(const World& w) const;

  private:
    void create(World& w, EntityTypeId type, Point p);

    util::CircularBuffer<EntityTypeId, MaxProductionQueueSize> queue_;
    SideId side_;
    ResourceBank resources_;
  };
}
