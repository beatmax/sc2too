#pragma once

#include <cstdint>
#include <iosfwd>
#include <sys/types.h>

namespace rts {

  struct Location {
    size_t x;
    size_t y;

    bool operator==(const Location& other) const { return x == other.x && y == other.y; }
  };

  struct Vector {
    ssize_t x;
    ssize_t y;
  };

  inline Vector operator-(const Location& a, const Location& b) {
    return {ssize_t(a.x - b.x), ssize_t(a.y - b.y)};
  }

  std::ostream& operator<<(std::ostream& os, const Location& loc);
}
