#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <sys/types.h>

namespace rts {

  class Entity;
  class World;

  using EntitySPtr = std::shared_ptr<Entity>;
  using EntitySCPtr = std::shared_ptr<const Entity>;
  using EntityWPtr = std::weak_ptr<Entity>;
  using EntityWCPtr = std::weak_ptr<const Entity>;

  using GameTime = uint32_t;  // = centiseconds in normal speed (100 Hz)
  using Coordinate = ssize_t;

  struct Position {
    Coordinate x;
    Coordinate y;

    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position& other) const { return !(*this == other); }
  };

  struct Vector {
    Coordinate x;
    Coordinate y;
  };

  inline Position operator+(const Position& p, const Vector& v) { return {p.x + v.x, p.y + v.y}; }
  inline Position& operator+=(Position& p, const Vector& v) { return p = p + v; }
  inline Vector operator-(const Position& a, const Position& b) { return {a.x - b.x, a.y - b.y}; }

  std::ostream& operator<<(std::ostream& os, const Position& p);
}
