#pragma once

#include <iosfwd>
#include <sys/types.h>

namespace util::geo {
  using Coordinate = ssize_t;

  struct Point {
    Coordinate x;
    Coordinate y;

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
  };

  struct Vector {
    Coordinate x;
    Coordinate y;

    bool operator==(const Vector& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector& other) const { return !(*this == other); }
  };

  struct Rectangle {
    Point topLeft;
    Vector size;

    bool operator==(const Rectangle& other) const {
      return topLeft == other.topLeft && size == other.size;
    }
    bool operator!=(const Rectangle& other) const { return !(*this == other); }

    bool contains(const Point& p) const {
      return p.x >= topLeft.x && p.y >= topLeft.y && p.x < (topLeft.x + size.x) &&
          p.y < (topLeft.y + size.y);
    }
  };

  inline Point operator+(const Point& p, const Vector& v) { return {p.x + v.x, p.y + v.y}; }
  inline Point& operator+=(Point& p, const Vector& v) { return p = p + v; }
  inline Point operator-(const Point& p, const Vector& v) { return {p.x - v.x, p.y - v.y}; }
  inline Point& operator-=(Point& p, const Vector& v) { return p = p - v; }
  inline Vector operator-(const Point& a, const Point& b) { return {a.x - b.x, a.y - b.y}; }
  inline Vector operator+(const Vector& v1, const Vector& v2) { return {v1.x + v2.x, v1.y + v2.y}; }

  template<typename F>
  void forEachPoint(const Rectangle& rect, F&& f) {
    for (auto x = rect.topLeft.x; x < rect.topLeft.x + rect.size.x; ++x)
      for (auto y = rect.topLeft.y; y < rect.topLeft.y + rect.size.y; ++y)
        f(Point{x, y});
  }

  std::ostream& operator<<(std::ostream& os, const Point& p);
  std::ostream& operator<<(std::ostream& os, const Vector& v);
  std::ostream& operator<<(std::ostream& os, const Rectangle& r);
}
