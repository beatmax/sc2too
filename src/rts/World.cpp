#include "rts/World.h"

#include "rts/Path.h"
#include "rts/SemaphoreLock.h"
#include "rts/Unit.h"
#include "rts/WorldAction.h"
#include "util/Overloaded.h"
#include "util/algorithm.h"
#include "util/geo.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace rts {
  namespace {
    template<typename Id>
    WorldObject* objectPtr(World& w, Id id) {
      return &w[id];
    }

    WorldObject* objectPtr(World&, Cell::Empty) { return nullptr; }

    Direction direction(const Vector& v) {
      return v.x < 0 ? Direction::Left
          : v.x > 0  ? Direction::Right
          : v.y < 0  ? Direction::Up
          : v.y > 0  ? Direction::Down
                     : Direction::Right;
    }
  }
}

rts::World::World(FactoryUPtr f) : factory{std::move(f)} {
  factory->init(*this);
}

rts::World::~World() {
  for (auto& u : units)
    destroy(u);
  for (auto& rf : resourceFields)
    destroy(rf);
}

void rts::World::loadMap(const MapInitializer& init, const std::string& fpath) {
  map.load(*this, init, fpath);
}

void rts::World::loadMap(const MapInitializer& init, std::istream&& is) {
  map.load(*this, init, std::move(is));
}

void rts::World::loadMap(const MapInitializer& init, const std::vector<std::string>& lines) {
  map.load(*this, init, lines);
}

void rts::World::step() {
  ++time;
  ++energyIncreaseTimer;

  WorldActionList actions;
  for (auto& u : units)
    actions += Unit::step(StableRef{u}, *this);

  update(actions);
}

void rts::World::exec(const SideCommand& cmd) {
  update(sides[cmd.side].exec(*this, cmd.command));
}

void rts::World::update(const WorldActionList& actions) {
  triggerGroups.clear();
  for (const auto& action : actions)
    action(*this);
}

rts::BlockerId rts::World::add(BlockerId id, Point p) {
  auto& obj{(*this)[id]};
  obj.area.topLeft = p;
  map.setContent(obj.area, id);
  minimap.update(map, obj, 1);
  return id;
}

rts::ResourceFieldId rts::World::add(ResourceFieldId id, Point p) {
  auto& obj{(*this)[id]};
  obj.area.topLeft = p;
  map.setContent(obj.area, id);
  minimap.update(map, obj, 1);
  resourceProximityMap.updateContour(*this, obj.area, ResourceProximityRadius, 1);
  if (obj.requiresBuilding == ResourceField::RequiresBuilding::No) {
    resourceBaseSaturationMap.update(
        *this, Circle{obj.area.center(), ResourceBaseSaturationRadius}, obj.optimalWorkerCount);
  }
  return id;
}

rts::UnitId rts::World::add(UnitId id, Point p, bool allocCheck) {
  auto& obj{(*this)[id]};
  obj.allocate(*this, allocCheck);
  obj.activate(*this, p);
  return id;
}

rts::UnitId rts::World::addForFree(UnitId id, Point p) {
  auto& obj{(*this)[id]};
  auto& res{sides[obj.side].resources()};
  const auto& cost{unitTypes[obj.type].cost};
  res.provision(cost);
  add(id, p, false);
  res.deprovision(cost, AllocFilter::Recoverable);
  return id;
}

void rts::World::place(Unit& u) {
  assert(
      map.isEmpty(u.area) ||
      (u.resourceField && u.resourceField == resourceFieldId(u.area.topLeft)));
  map.setContent(u.area, id(u));
  minimap.update(map, u, 1);
  u.layer = LayerUnits;
  if (u.isStructure(*this))
    initMapSegments(*this);
}

bool rts::World::tryPlace(Unit& u) {
  if (map.isEmpty(u.area) ||
      (u.resourceField && u.resourceField == resourceFieldId(u.area.topLeft))) {
    place(u);
    return true;
  }
  return false;
}

void rts::World::remove(Unit& u) {
  assert(map[u.area.topLeft].contains(Cell::Unit));
  if (u.resourceField)
    map.setContent(u.area, u.resourceField);
  else
    map.setContent(u.area, Cell::Empty{});
  minimap.update(map, u, -1);
  if (u.isStructure(*this))
    initMapSegments(*this);
}

void rts::World::move(Unit& u, Point p) {
  assert(u.area.size == Vector({1, 1}));  // no need to move big units so far
  assert(map[p].empty());
  auto& epos = u.area.topLeft;
  u.direction = direction(p - epos);
  Cell& src{map[epos]};
  Cell& dst{map[p]};
  src.content = Cell::Empty{};
  dst.content = id(u);
  if (src.minimapCell == dst.minimapCell) {
    epos = p;
  }
  else {
    minimap.update(map, u, -1);
    epos = p;
    minimap.update(map, u, 1);
  }
}

void rts::World::destroy(Unit& u) {
  auto& s{sides[u.side]};
  u.destroy(*this);
  units.erase(id(u));
  s.onUnitDestroyed(*this);
}

void rts::World::destroy(ResourceField& rf) {
  assert(map[rf.area.topLeft].contains(Cell::ResourceField));
  minimap.update(map, rf, -1);
  resourceProximityMap.updateContour(*this, rf.area, ResourceProximityRadius, -1);
  if (rf.requiresBuilding == ResourceField::RequiresBuilding::No) {
    resourceBaseSaturationMap.update(
        *this, Circle{rf.area.center(), ResourceBaseSaturationRadius}, -rf.optimalWorkerCount);
  }
  map.setContent(rf.area, Cell::Empty{});
  resourceFields.erase(id(rf));
}

void rts::World::destroy(ProductionQueue& pq) {
  pq.onDestroy(*this);
  productionQueues.erase(id(pq));
}

const rts::Producible& rts::World::operator[](ProducibleId product) const {
  return std::visit([this](auto id) -> const Producible& { return (*this)[id]; }, product);
}

rts::AbilityTarget rts::World::abilityTarget(Point p) const {
  if (p.x < 0)
    return {};
  assert(inBounds(p));
  auto& cell{map[p]};
  if (auto u{cell.unitId()})
    return u;
  else if (auto rf{cell.resourceFieldId()})
    return rf;
  return p;
}

rts::AbilityWeakTarget rts::World::abilityWeakTarget(Point p) const {
  return abilityWeakTarget(abilityTarget(p));
}

rts::AbilityWeakTarget rts::World::abilityWeakTarget(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](std::monostate) -> AbilityWeakTarget { return {}; },
          [](Point p) -> AbilityWeakTarget { return p; },
          [this](auto id) -> AbilityWeakTarget { return weakId(id); }},
      t);
}

bool rts::World::compatibleTarget(rts::abilities::TargetType tt, SideId side, Point p) const {
  using rts::abilities::TargetType;
  auto rc{relativeContent(side, p)};
  switch (tt) {
    case TargetType::None:
      return false;
    case TargetType::Any:
      return true;
    case TargetType::Ground:
      return rc == RelativeContent::Ground;
    case TargetType::ProductionOrResearch:
      return rc == RelativeContent::Friend && unit(p)->productionQueue;
    case TargetType::Resource:
      return rc == RelativeContent::Resource || rc == RelativeContent::FriendResource;
  }
  return false;
}

rts::RelativeContent rts::World::relativeContent(SideId side, Point p) const {
  auto& cell{map[p]};
  if (auto* u{unit(cell)})
    return (u->side == side) ? ((u->resourceField && u->active()) ? RelativeContent::FriendResource
                                                                  : RelativeContent::Friend)
                             : RelativeContent::Foe;
  else if (auto* rf{resourceField(cell)};
           rf && rf->requiresBuilding == ResourceField::RequiresBuilding::No)
    return RelativeContent::Resource;
  return RelativeContent::Ground;
}

rts::RelativeContent rts::World::relativeContentForRally(SideId side, Point p) const {
  auto rc{relativeContent(side, p)};
  switch (rc) {
    case RelativeContent::Resource:
    case RelativeContent::FriendResource:
      return rc;
    default:
      return RelativeContent::Ground;
  }
}

rts::ResourceFieldId rts::World::resourceFieldId(Point p, SideId side) const {
  auto& cell{map[p]};
  if (auto* u{unit(cell)})
    return (u->resourceField && u->side == side) ? u->resourceField : ResourceFieldId{};
  else if (auto* rf{resourceField(cell)};
           rf && rf->requiresBuilding == ResourceField::RequiresBuilding::No)
    return id(*rf);
  return {};
}

rts::ResourceField* rts::World::resourceField(Point p, SideId side) {
  if (auto id{resourceFieldId(p, side)})
    return &(*this)[id];
  return nullptr;
}

rts::ResourceField* rts::World::resourceField(const AbilityWeakTarget& t, SideId side) {
  if (auto target{fromWeakTarget(t)}) {
    if (auto* rf{resourceField(center(*target), side)})
      return rf;
  }
  return nullptr;
}

rts::WorldObject* rts::World::object(const Cell& c) {
  return std::visit([this](auto id) { return objectPtr(*this, id); }, c.content);
}

std::set<rts::WorldObjectCPtr> rts::World::objectsInArea(const Rectangle& area) const {
  return objectsInArea(area, map);
}

std::set<rts::WorldObjectCPtr> rts::World::objectsInArea(
    const Rectangle& area, const Map& m) const {
  std::set<WorldObjectCPtr> result;
  for (Point p : area.points()) {
    if (auto obj{object(m[p])})
      result.insert(obj);
  }
  return result;
}

rts::UnitIdList rts::World::unitsInArea(const Rectangle& area, SideId side, UnitTypeId type) const {
  UnitIdList result;
  for (Point p : area.points()) {
    if (auto uId{unitId(p)}) {
      if (!util::contains(result, uId)) {
        const auto& u{units[uId]};
        if (u.activeOrBuilding() && (!side || u.side == side) && (!type || u.type == type))
          result.push_back(uId);
      }
    }
  }
  return result;
}

rts::UnitIdList rts::World::unitsInAreaForSelection(const Rectangle& area, SideId side) const {
  UnitIdList result;
  bool structures{false}, others{false};
  for (Point p : area.points()) {
    if (auto uId{unitId(p)}) {
      if (!util::contains(result, uId)) {
        const auto& u{units[uId]};
        bool isStruct{u.isStructure(*this)};
        if (!(isStruct && others) && u.activeOrBuilding() && (!side || u.side == side)) {
          result.push_back(uId);
          structures |= isStruct;
          others |= !isStruct;
        }
      }
    }
  }
  if (structures && others) {
    result.erase(
        std::remove_if(
            result.begin(), result.end(), [this](auto id) { return units[id].isStructure(*this); }),
        result.end());
  }
  return result;
}

rts::UnitIdList rts::World::unitsInPoints(
    const PointList& points, SideId side, UnitType::Kind kind) const {
  UnitIdList result;
  for (Point p : points) {
    if (auto uId{unitId(p)}) {
      if (!util::contains(result, uId)) {
        const auto& u{units[uId]};
        const auto& t{unitTypes[u.type]};
        if (u.activeOrBuilding() && (!side || u.side == side) && t.kind == kind)
          result.push_back(uId);
      }
    }
  }
  return result;
}

rts::Unit* rts::World::closestActiveUnit(Point p, SideId side, UnitTypeId type) {
  Unit* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (auto& u : units) {
    if (u.type == type && u.side == side && u.active()) {
      if (float d{diagonalDistance(p, u.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &u;
      }
    }
  }
  return closest;
}

const rts::ResourceField* rts::World::closestResourceField(
    Point p, ResourceGroupId group, bool blockedOk) const {
  assert(group);
  const ResourceField* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& rf : resourceFields) {
    if (rf.group == group && (blockedOk || !rf.sem.blocked())) {
      if (float d{diagonalDistance(p, rf.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &rf;
      }
    }
  }
  return closest;
}

const rts::ResourceField* rts::World::closestResourceField(Point p, ResourceId r) const {
  const ResourceField* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (const auto& rf : resourceFields) {
    if (rf.bag.resource == r) {
      if (float d{diagonalDistance(p, rf.area.center())}; d < closestDistance) {
        closestDistance = d;
        closest = &rf;
      }
    }
  }
  return closest;
}

std::optional<rts::Point> rts::World::emptyCellAround(const Rectangle& area, Point towards) const {
  auto points{intersection(map.area(), boundingBox(area)).outerPoints()};
  auto it{util::minElementBy(points, [&](Point p) {
    return map[p].empty() ? diagonalDistance(p, towards) : std::numeric_limits<float>::max();
  })};
  if (it != points.end() && map[*it].empty())
    return *it;
  return std::nullopt;
}

rts::Point rts::World::centralPoint(const UnitCPtrList& units) {
  assert(!units.empty());
  return toPoint(
      std::accumulate(
          units.begin(), units.end(), Vector{0, 0},
          [](Vector v, UnitCPtr u) { return v + toVector(u->area.center()); }) /
      Coordinate(units.size()));
}

const rts::Unit& rts::World::centralUnit(const UnitCPtrList& units) {
  auto p{centralPoint(units)};
  return **util::minElementBy(
      units, [p](UnitCPtr u) { return diagonalDistance(p, u->area.center()); });
}

rts::Rectangle rts::World::area(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](std::monostate) {
            assert(false);
            return Rectangle{};
          },
          [](Point p) {
            return Rectangle{p, {1, 1}};
          },
          [this](auto id) { return (*this)[id].area; }},
      t);
}

rts::Point rts::World::center(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](std::monostate) {
            assert(false);
            return Point{};
          },
          [](Point p) { return p; }, [this](auto id) { return (*this)[id].area.center(); }},
      t);
}

std::optional<rts::AbilityTarget> rts::World::fromWeakTarget(const AbilityWeakTarget& t) const {
  using OptTarget = std::optional<rts::AbilityTarget>;
  return std::visit(
      util::Overloaded{
          [](std::monostate) -> OptTarget { return AbilityTarget{}; },
          [](Point p) -> OptTarget { return p; },
          [this](auto wid) -> OptTarget {
            auto* obj{(*this)[wid]};
            return obj ? OptTarget{id(*obj)} : std::nullopt;
          }},
      t);
}

auto rts::World::targets(const UnitCPtrList& units) const
    -> std::pair<PointList, std::set<WorldObjectCPtr>> {
  PointList points;
  std::set<WorldObjectCPtr> objects;
  for (const auto* u : units) {
    for (size_t i{0}; i < u->commandQueue.size(); ++i) {
      const UnitCommand& cmd{u->commandQueue[i]};
      if (!cmd.prototype) {
        if (auto target{fromWeakTarget(cmd.target)}) {
          std::visit(
              util::Overloaded{
                  [](std::monostate) {}, [&points](Point p) { points.push_back(p); },
                  [this, &objects](auto id) { objects.insert(&(*this)[id]); }},
              *target);
        }
      }
    }
    if (u->productionQueue) {
      const ProductionQueue& pq{(*this)[u->productionQueue]};
      if (auto p{pq.rallyPoint()}) {
        if (auto* obj{object(*p)})
          objects.insert(obj);
        else
          points.push_back(*p);
      }
    }
  }
  return {std::move(points), std::move(objects)};
}

void rts::World::onMapCreated() {
  minimap.initMapCells(map);
  resourceProximityMap.initCells(map.size());
  resourceBaseSaturationMap.initCells(map.size());
  for (auto& s : sides)
    s.onMapCreated(*this);
}

void rts::World::onMapLoaded() {
  initMapSegments(*this);
}
