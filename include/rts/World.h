#pragma once

#include "Ability.h"
#include "Blocker.h"
#include "Entity.h"
#include "EntityType.h"
#include "Map.h"
#include "ResourceField.h"
#include "Side.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"
#include "util/Pool.h"

#include <memory>
#include <set>
#include <utility>

namespace rts {

  class World {
  public:
    // note: do not create a World on the stack
    static std::unique_ptr<World> create() { return std::make_unique<World>(); }

    GameTime time{};
    util::Pool<Side, MaxSides> sides;
    util::Pool<Entity, MaxEntities> entities;
    util::Pool<EntityType, MaxEntityTypes> entityTypes;
    util::Pool<Ability, MaxAbilities> abilities;
    util::Pool<Blocker, MaxBlockers> blockers;
    util::Pool<ResourceField, MaxResourceFields> resourceFields;
    Map map;

    World() = default;
    ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    void update(const WorldActionList& actions);

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

    void destroy(const ResourceField& rf);
    void destroy(ResourceFieldId rf) { destroy(resourceFields[rf]); }

    Side& operator[](SideId id) { return sides[id]; }
    const Side& operator[](SideId id) const { return sides[id]; }
    Entity& operator[](EntityId id) { return entities[id]; }
    const Entity& operator[](EntityId id) const { return entities[id]; }
    EntityType& operator[](EntityTypeId id) { return entityTypes[id]; }
    const EntityType& operator[](EntityTypeId id) const { return entityTypes[id]; }
    Ability& operator[](AbilityId id) { return abilities[id]; }
    const Ability& operator[](AbilityId id) const { return abilities[id]; }
    Blocker& operator[](BlockerId id) { return blockers[id]; }
    const Blocker& operator[](BlockerId id) const { return blockers[id]; }
    ResourceField& operator[](ResourceFieldId id) { return resourceFields[id]; }
    const ResourceField& operator[](ResourceFieldId id) const { return resourceFields[id]; }

    Entity* operator[](EntityWId wid) { return entities[wid]; }
    const Entity* operator[](EntityWId wid) const { return entities[wid]; }
    ResourceField* operator[](ResourceFieldWId wid) { return resourceFields[wid]; }
    const ResourceField* operator[](ResourceFieldWId wid) const { return resourceFields[wid]; }

    EntityWId weakId(const Entity& e) const { return entities.weakId(e); }
    ResourceFieldWId weakId(const ResourceField& rf) const { return resourceFields.weakId(rf); }

    Cell& operator[](Point p) { return map[p]; }
    const Cell& operator[](Point p) const { return map[p]; }

    RelativeContent relativeContent(SideId side, Point p) const;

    BlockerId blockerId(Point p) const { return map[p].blockerId(); }
    EntityId entityId(Point p) const { return map[p].entityId(); }
    ResourceFieldId resourceFieldId(Point p) const { return map[p].resourceFieldId(); }

    Blocker* blocker(const Cell& c) { return getPtrAt(c, blockers); }
    const Blocker* blocker(const Cell& c) const { return getPtrAt(c, blockers); }
    Blocker* blocker(Point p) { return blocker(map[p]); }
    const Blocker* blocker(Point p) const { return blocker(map[p]); }

    Entity* entity(const Cell& c) { return getPtrAt(c, entities); }
    const Entity* entity(const Cell& c) const { return getPtrAt(c, entities); }
    Entity* entity(Point p) { return entity(map[p]); }
    const Entity* entity(Point p) const { return entity(map[p]); }

    ResourceField* resourceField(const Cell& c) { return getPtrAt(c, resourceFields); }
    const ResourceField* resourceField(const Cell& c) const { return getPtrAt(c, resourceFields); }
    ResourceField* resourceField(Point p) { return resourceField(map[p]); }
    const ResourceField* resourceField(Point p) const { return resourceField(map[p]); }

    WorldObject* object(const Cell& c);
    const WorldObject* object(const Cell& c) const { return const_cast<World&>(*this).object(c); }
    WorldObject* object(Point p) { return object(map[p]); }
    const WorldObject* object(Point p) const { return object(map[p]); }

    std::set<WorldObjectCPtr> objectsInArea(const Rectangle& area) const;
    EntityIdList entitiesInArea(
        const Rectangle& area, SideId side = {}, EntityTypeId type = {}) const;
    const Entity* closestEntity(Point p, SideId side, EntityTypeId type) const;
    const ResourceField* closestResourceField(Point p, ResourceGroupId group, bool blockedOk) const;

  private:
    void update(const action::CommandAction& action);
    void update(const action::AbilityStepAction& action);
    void update(const action::ResourceFieldReleaseAction& action);

    template<typename P, typename... Args>
    auto create(P& pool, Args&&... args) {
      auto id{pool.emplace(std::forward<Args>(args)...)};
      map.setContent(pool[id].area, id);
      return id;
    }

    template<typename T, uint32_t N>
    static T* getPtrAt(const Cell& c, util::Pool<T, N>& pool) {
      using Id = util::PoolObjectId<T>;
      Id id{c.getId<Id>()};
      return id ? &pool[id] : nullptr;
    }

    template<typename T, uint32_t N>
    static const T* getPtrAt(const Cell& c, const util::Pool<T, N>& pool) {
      using Pool = util::Pool<T, N>;
      return getPtrAt(c, const_cast<Pool&>(pool));
    }
  };
}
