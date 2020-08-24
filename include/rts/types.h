#pragma once

#include "util/Pool.h"
#include "util/geo.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <variant>
#include <vector>

namespace rts {

  class Ability;
  class Blocker;
  class Engine;
  class Factory;
  class Map;
  class ProductionQueue;
  class Resource;
  class ResourceField;
  class Selection;
  class Side;
  class Unit;
  class UnitType;
  class World;
  class WorldObject;

  template<typename Tag>
  class NamedIndex {
  public:
    explicit constexpr NamedIndex(size_t i) : i_{i} {}
    static constexpr NamedIndex None{std::numeric_limits<size_t>::max()};
    operator size_t() const { return i_; }

  private:
    size_t i_;
  };

  // const reference to object whose mutable members are only modified by the current thread
  template<typename T>
  class StableRef {
  public:
    explicit StableRef(const T& t) : t_{t} {}
    operator const T&() const { return t_; }
    const T& operator*() const { return t_; }
    const T* operator->() const { return &t_; }

  private:
    const T& t_;
  };

  using AbilityId = util::PoolObjectId<Ability>;
  using AbilityInstanceIndex = NamedIndex<struct AbilityInstanceIndexTag>;
  using AbilityStateIndex = NamedIndex<struct AbilityStateIndexTag>;
  using BlockerId = util::PoolObjectId<Blocker>;
  using BlockerStableRef = StableRef<Blocker>;
  using BlockerWId = util::PoolObjectWeakId<Blocker>;
  using ControlGroupId = uint8_t;
  using ProductionQueueId = util::PoolObjectId<ProductionQueue>;
  using ProductionQueueWId = util::PoolObjectWeakId<ProductionQueue>;
  using ResourceFieldId = util::PoolObjectId<ResourceField>;
  using ResourceFieldStableRef = StableRef<ResourceField>;
  using ResourceFieldWId = util::PoolObjectWeakId<ResourceField>;
  using ResourceGroupId = uint32_t;
  using ResourceId = util::PoolObjectId<Resource>;
  using SideId = util::PoolObjectId<Side>;
  using UnitCPtr = const Unit*;
  using UnitCPtrList = std::vector<UnitCPtr>;
  using UnitId = util::PoolObjectId<Unit>;
  using UnitIdList = std::vector<UnitId>;
  using UnitStableRef = StableRef<Unit>;
  using UnitTypeId = util::PoolObjectId<UnitType>;
  using UnitWId = util::PoolObjectWeakId<Unit>;
  using WorldObjectCPtr = const WorldObject*;

  using AnyWeakId = std::variant<UnitWId, ProductionQueueWId>;

  using Distance = uint32_t;   // = 1/100 cell width/height
  using GameTime = uint32_t;   // = centiseconds at normal speed (100 Hz)
  using GameSpeed = uint32_t;  // game units per second
  using Speed = uint32_t;      // distance per game unit = cells per second
  using Fps = uint32_t;
  using Quantity = uint32_t;

  using ResourceQuantityList = std::vector<std::pair<ResourceId, Quantity>>;

  using util::geo::Coordinate;
  using util::geo::Point;
  using util::geo::Rectangle;
  using util::geo::Vector;

  enum class RelativeContent : size_t { Friend, Foe, Ground, Resource, Count };

  class Ui {
  public:
    virtual ~Ui() = default;
  };

  using FactoryUPtr = std::unique_ptr<Factory>;
  using UiUPtr = std::unique_ptr<Ui>;
}
