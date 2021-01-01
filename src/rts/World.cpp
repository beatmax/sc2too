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

void rts::World::loadMap(const MapInitializer& init, std::istream&& is) {
  map.load(*this, init, std::move(is));
  onMapLoaded();
}

void rts::World::loadMap(const MapInitializer& init, const std::vector<std::string>& lines) {
  map.load(*this, init, lines);
  onMapLoaded();
}

void rts::World::exec(const SideCommand& cmd) {
  update(sides[cmd.side].exec(*this, cmd.command));
}

void rts::World::update(const WorldActionList& actions) {
  triggerGroups.clear();
  for (const auto& action : actions)
    action(*this);
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
  if (u.isStructure(*this))
    initMapSegments(*this);
}

void rts::World::move(Unit& u, Point p) {
  assert(u.area.size == Vector({1, 1}));  // no need to move big units so far
  assert(map[p].empty());
  auto& epos = u.area.topLeft;
  u.direction = direction(p - epos);
  map.setContent(epos, Cell::Empty{});
  map.setContent(p, id(u));
  epos = p;
}

void rts::World::destroy(Unit& u) {
  auto& s{sides[u.side]};
  u.destroy(*this);
  units.erase(id(u));
  s.onUnitDestroyed(*this);
}

void rts::World::destroy(ResourceField& rf) {
  assert(map[rf.area.topLeft].contains(Cell::ResourceField));
  map.setContent(rf.area, Cell::Empty{});
  resourceFields.erase(id(rf));
}

void rts::World::destroy(ProductionQueue& pq) {
  pq.onDestroy(*this);
  productionQueues.erase(id(pq));
}

rts::AbilityTarget rts::World::abilityTarget(Point p) const {
  auto& cell{map[p]};
  if (auto u{cell.unitId()})
    return u;
  else if (auto rf{cell.resourceFieldId()})
    return rf;
  return p;
}

rts::AbilityWeakTarget rts::World::abilityWeakTarget(Point p) const {
  auto& cell{map[p]};
  if (auto u{cell.unitId()})
    return weakId(u);
  else if (auto rf{cell.resourceFieldId()})
    return weakId(rf);
  return p;
}

rts::AbilityWeakTarget rts::World::abilityWeakTarget(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](Point p) -> AbilityWeakTarget { return p; },
          [this](auto id) -> AbilityWeakTarget { return weakId(id); }},
      t);
}

rts::RelativeContent rts::World::relativeContent(SideId side, Point p) const {
  auto& cell{map[p]};
  if (auto* u{unit(cell)})
    return (u->side == side)
        ? (u->resourceField ? RelativeContent::FriendResource : RelativeContent::Friend)
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

rts::Unit* rts::World::closestActiveUnit(Point p, SideId side, UnitTypeId type) {
  Unit* closest{nullptr};
  float closestDistance{std::numeric_limits<float>::infinity()};
  for (auto& u : units) {
    if (u.active() && u.side == side && u.type == type) {
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
  auto points{util::geo::boundingBox(area).outerPoints()};
  auto it{util::minElementBy(points, [&](Point p) {
    return map[p].empty() ? diagonalDistance(p, towards) : std::numeric_limits<float>::max();
  })};
  if (it != points.end() && map[*it].empty())
    return *it;
  return std::nullopt;
}

rts::Point rts::World::centralPoint(const UnitCPtrList& units) {
  assert(!units.empty());
  return Point{0, 0} +
      std::accumulate(
          units.begin(), units.end(), Vector{0, 0},
          [](Vector v, UnitCPtr u) {
            return v + (u->area.center() - Point{0, 0});
          }) /
      Coordinate(units.size());
}

const rts::Unit& rts::World::centralUnit(const UnitCPtrList& units) {
  auto p{centralPoint(units)};
  return **util::minElementBy(
      units, [p](UnitCPtr u) { return diagonalDistance(p, u->area.center()); });
}

rts::Rectangle rts::World::area(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](Point p) {
            return Rectangle{p, {1, 1}};
          },
          [this](auto id) { return (*this)[id].area; }},
      t);
}

rts::Point rts::World::center(const AbilityTarget& t) const {
  return std::visit(
      util::Overloaded{
          [](Point p) { return p; }, [this](auto id) { return (*this)[id].area.center(); }},
      t);
}

std::optional<rts::AbilityTarget> rts::World::fromWeakTarget(const AbilityWeakTarget& t) const {
  using OptTarget = std::optional<rts::AbilityTarget>;
  return std::visit(
      util::Overloaded{
          [](Point p) -> OptTarget { return p; },
          [this](auto wid) -> OptTarget {
            auto* obj{(*this)[wid]};
            return obj ? OptTarget{id(*obj)} : std::nullopt;
          }},
      t);
}

void rts::World::onMapLoaded() {
  initMapSegments(*this);
  for (auto& s : sides)
    s.onMapLoaded(*this);
}
