#pragma once

#include "Resource.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"

#include <array>

namespace rts {
  class ProductionQueue {
  public:
    explicit ProductionQueue(SideId side) : side_{side} {};

    bool add(World& w, EntityTypeId type);
    bool startProduction(World& w);
    bool finishProduction(World& w, const Entity& parent);

    SideId side() const { return side_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    EntityTypeId top() const { return type(0); }
    EntityTypeId type(size_t index) const { return array_[index]; }
    GameTime buildTime(const World& w) const;

  private:
    using Array = std::array<EntityTypeId, MaxProductionQueueSize>;

    void create(World& w, EntityTypeId type, Point p);
    void pop();

    SideId side_;
    Array array_;
    size_t size_;
    ResourceBank resources_;
  };
}
