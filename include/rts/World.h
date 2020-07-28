#pragma once

#include "Map.h"
#include "Side.h"
#include "WorldAction.h"
#include "types.h"
#include "util/Pool.h"

#include <memory>

namespace rts {

  class World {
  public:
    constexpr static size_t MaxSides{8};
    constexpr static size_t MaxSideUnits{200};
    constexpr static size_t MaxSideStructures{1000};
    constexpr static size_t MaxSideEntities{MaxSideUnits + MaxSideStructures};
    constexpr static size_t MaxEntities{MaxSideEntities * MaxSides};
    constexpr static size_t MaxBlockers{10000};
    constexpr static size_t MaxResourceFields{10000};

    // note: do not create a World on the stack
    static std::unique_ptr<World> create() { return std::make_unique<World>(); }

    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    GameTime time{};
    util::Pool<Side, MaxSides> sides;
    util::Pool<Entity, MaxEntities> entities;
    util::Pool<Blocker, MaxBlockers> blockers;
    util::Pool<ResourceField, MaxResourceFields> resourceFields;
    Map map;

    void update(const WorldActionList& actions);
    void update(const action::AbilityStepAction& action);

    template<typename... Args>
    SideId createSide(Args&&... args) {
      return sides.emplace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    EntityId createEntity(Args&&... args) {
      return create(entities, std::forward<Args>(args)...);
    }

    template<typename... Args>
    BlockerId createBlocker(Args&&... args) {
      return create(blockers, std::forward<Args>(args)...);
    }

    template<typename... Args>
    ResourceFieldId createResourceField(Args&&... args) {
      return create(resourceFields, std::forward<Args>(args)...);
    }

    void move(Entity& e, Point p);

    void destroy(const Entity& e);
    void destroy(EntityId e) { destroy(entities[e]); }

    Entity& operator[](EntityId id) { return entities[id]; }
    const Entity& operator[](EntityId id) const { return entities[id]; }
    Blocker& operator[](BlockerId id) { return blockers[id]; }
    const Blocker& operator[](BlockerId id) const { return blockers[id]; }
    ResourceField& operator[](ResourceFieldId id) { return resourceFields[id]; }
    const ResourceField& operator[](ResourceFieldId id) const { return resourceFields[id]; }
    WorldObject& operator[](Map::Free);
    const WorldObject& operator[](Map::Free) const;

    WorldObject& objectAt(Point p);
    const WorldObject& objectAt(Point p) const;

    WorldObject* objectPtrAt(Point p);
    const WorldObject* objectPtrAt(Point p) const;

    Entity& entityAt(Point p);
    const Entity& entityAt(Point p) const;

    Blocker& blockerAt(Point p);
    const Blocker& blockerAt(Point p) const;

    ResourceField& resourceFieldAt(Point p);
    const ResourceField& resourceFieldAt(Point p) const;

  private:
    template<typename P, typename... Args>
    auto create(P& pool, Args&&... args) {
      auto id{pool.emplace(std::forward<Args>(args)...)};
      map.setContent(pool[id].area, id);
      return id;
    }
  };
}
