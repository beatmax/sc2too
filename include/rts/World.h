#pragma once

#include "Map.h"
#include "Side.h"
#include "WorldAction.h"
#include "types.h"
#include "util/Pool.h"

#include <memory>
#include <vector>

namespace rts {

  class World {
  public:
    constexpr static size_t MaxSides{8};
    constexpr static size_t MaxEntities{200 * MaxSides};
    constexpr static size_t MaxBlockers{10000};
    constexpr static size_t MaxResourceFields{10000};

    // note: do not create on stack
    World(std::vector<Side> s, const MapInitializer& init, std::istream&& is);

    template<typename... Args>
    static std::unique_ptr<World> create(Args&&... args) {
      return std::make_unique<World>(std::forward<Args>(args)...);
    }

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    GameTime time{};

    std::vector<Side> sides;
    util::Pool<Entity, MaxEntities> entities;
    util::Pool<Blocker, MaxBlockers> blockers;
    util::Pool<ResourceField, MaxResourceFields> resourceFields;
    Map map;

    void update(const WorldActionList& actions);
    void update(const action::AbilityStepAction& action);

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
