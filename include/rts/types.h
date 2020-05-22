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

  struct Position {
    size_t x;
    size_t y;

    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position& other) const { return !(*this == other); }
  };

  struct Vector {
    ssize_t x;
    ssize_t y;
  };

  inline Position operator+(const Position& p, const Vector& v) { return {p.x + v.x, p.y + v.y}; }

  inline Vector operator-(const Position& a, const Position& b) {
    return {ssize_t(a.x - b.x), ssize_t(a.y - b.y)};
  }

  std::ostream& operator<<(std::ostream& os, const Position& p);
}
