#pragma once

#include "Ability.h"
#include "Blocker.h"
#include "Command.h"
#include "Factory.h"
#include "Map.h"
#include "ProductionQueue.h"
#include "Resource.h"
#include "ResourceField.h"
#include "Side.h"
#include "Unit.h"
#include "UnitType.h"
#include "WorldAction.h"
#include "constants.h"
#include "types.h"
#include "util/Pool.h"

#include <memory>
#include <optional>
#include <set>
#include <utility>

namespace rts {

  class World {
  public:
    // note: do not create a World on the stack
    static std::unique_ptr<World> create(FactoryUPtr factory) {
      return std::make_unique<World>(std::move(factory));
    }

    Map map;
    GameTime time{};
    FactoryUPtr factory;

    util::Pool<Resource, MaxResources> resources;
    util::Pool<Ability, MaxAbilities> abilities;
    util::Pool<UnitType, MaxUnitTypes> unitTypes;
    util::Pool<Side, MaxSides> sides;
    util::Pool<Unit, MaxUnits> units;
    util::Pool<ResourceField, MaxResourceFields> resourceFields;
    util::Pool<Blocker, MaxBlockers> blockers;
    util::Pool<ProductionQueue, MaxProductionQueues> productionQueues;

    explicit World(FactoryUPtr f);
    ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    void exec(const SideCommandList& commands);
    void update(const WorldActionList& actions);

    template<typename... Args>
    ResourceId createResource(Args&&... args) {
      return resources.emplace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    AbilityId createAbility(Args&&... args) {
      return abilities.emplace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    UnitTypeId createUnitType(Args&&... args) {
      return unitTypes.emplace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    SideId createSide(Args&&... args) {
      return sides.emplace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    UnitId createUnit(Args&&... args) {
      return create(units, std::forward<Args>(args)...);
    }

    template<typename... Args>
    ResourceFieldId createResourceField(Args&&... args) {
      return create(resourceFields, std::forward<Args>(args)...);
    }

    template<typename... Args>
    BlockerId createBlocker(Args&&... args) {
      return create(blockers, std::forward<Args>(args)...);
    }

    template<typename... Args>
    ProductionQueueId createProductionQueue(Args&&... args) {
      return productionQueues.emplace(std::forward<Args>(args)...);
    }

    void move(Unit& u, Point p);

    void destroy(Unit& u);
    void destroy(UnitId u) { destroy(units[u]); }
    void destroy(ResourceField& rf);
    void destroy(ResourceFieldId rf) { destroy(resourceFields[rf]); }
    void destroy(ProductionQueue& pq) { destroy(productionQueues.id(pq)); }
    void destroy(ProductionQueueId pq) { productionQueues.erase(pq); }

    const Resource& operator[](ResourceId id) const { return resources[id]; }
    Ability& operator[](AbilityId id) { return abilities[id]; }
    const Ability& operator[](AbilityId id) const { return abilities[id]; }
    UnitType& operator[](UnitTypeId id) { return unitTypes[id]; }
    const UnitType& operator[](UnitTypeId id) const { return unitTypes[id]; }
    Side& operator[](SideId id) { return sides[id]; }
    const Side& operator[](SideId id) const { return sides[id]; }
    Unit& operator[](UnitId id) { return units[id]; }
    const Unit& operator[](UnitId id) const { return units[id]; }
    ResourceField& operator[](ResourceFieldId id) { return resourceFields[id]; }
    const ResourceField& operator[](ResourceFieldId id) const { return resourceFields[id]; }
    Blocker& operator[](BlockerId id) { return blockers[id]; }
    const Blocker& operator[](BlockerId id) const { return blockers[id]; }
    ProductionQueue& operator[](ProductionQueueId id) { return productionQueues[id]; }
    const ProductionQueue& operator[](ProductionQueueId id) const { return productionQueues[id]; }

    Unit* operator[](UnitWId wid) { return units[wid]; }
    const Unit* operator[](UnitWId wid) const { return units[wid]; }
    ResourceField* operator[](ResourceFieldWId wid) { return resourceFields[wid]; }
    const ResourceField* operator[](ResourceFieldWId wid) const { return resourceFields[wid]; }
    ProductionQueue* operator[](ProductionQueueWId wid) { return productionQueues[wid]; }
    const ProductionQueue* operator[](ProductionQueueWId wid) const {
      return productionQueues[wid];
    }

    AbilityId id(const Ability& a) const { return abilities.id(a); }
    SideId id(const Side& s) const { return sides.id(s); }
    UnitId id(const Unit& u) const { return units.id(u); }
    ResourceFieldId id(const ResourceField& rf) const { return resourceFields.id(rf); }
    BlockerId id(const Blocker& b) const { return blockers.id(b); }
    ProductionQueueId id(const ProductionQueue& pq) const { return productionQueues.id(pq); }

    UnitWId weakId(const Unit& u) const { return units.weakId(u); }
    ResourceFieldWId weakId(const ResourceField& rf) const { return resourceFields.weakId(rf); }
    ProductionQueueWId weakId(const ProductionQueue& pq) const {
      return productionQueues.weakId(pq);
    }
    ProductionQueueWId weakId(ProductionQueueId id) const { return weakId((*this)[id]); }

    Cell& operator[](Point p) { return map[p]; }
    const Cell& operator[](Point p) const { return map[p]; }

    RelativeContent relativeContent(SideId side, Point p) const;

    UnitId unitId(Point p) const { return map[p].unitId(); }
    ResourceFieldId resourceFieldId(Point p) const { return map[p].resourceFieldId(); }
    BlockerId blockerId(Point p) const { return map[p].blockerId(); }

    Unit* unit(const Cell& c) { return getPtrAt(c, units); }
    const Unit* unit(const Cell& c) const { return getPtrAt(c, units); }
    Unit* unit(Point p) { return unit(map[p]); }
    const Unit* unit(Point p) const { return unit(map[p]); }

    ResourceField* resourceField(const Cell& c) { return getPtrAt(c, resourceFields); }
    const ResourceField* resourceField(const Cell& c) const { return getPtrAt(c, resourceFields); }
    ResourceField* resourceField(Point p) { return resourceField(map[p]); }
    const ResourceField* resourceField(Point p) const { return resourceField(map[p]); }

    Blocker* blocker(const Cell& c) { return getPtrAt(c, blockers); }
    const Blocker* blocker(const Cell& c) const { return getPtrAt(c, blockers); }
    Blocker* blocker(Point p) { return blocker(map[p]); }
    const Blocker* blocker(Point p) const { return blocker(map[p]); }

    WorldObject* object(const Cell& c);
    const WorldObject* object(const Cell& c) const { return const_cast<World&>(*this).object(c); }
    WorldObject* object(Point p) { return object(map[p]); }
    const WorldObject* object(Point p) const { return object(map[p]); }

    std::set<WorldObjectCPtr> objectsInArea(const Rectangle& area) const;
    UnitIdList unitsInArea(const Rectangle& area, SideId side = {}, UnitTypeId type = {}) const;
    const Unit* closestUnit(Point p, SideId side, UnitTypeId type) const;
    const ResourceField* closestResourceField(Point p, ResourceGroupId group, bool blockedOk) const;
    std::optional<Point> emptyCellAround(const Rectangle& area) const;

  private:
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
