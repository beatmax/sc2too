#pragma once

#include <functional>
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
  inline Vector operator-(const Vector& v1, const Vector& v2) { return {v1.x - v2.x, v1.y - v2.y}; }
  inline Vector operator-(const Vector& v) { return {-v.x, -v.y}; }

  template<typename F>
  void forEachPoint(const Rectangle& rect, F&& f) {
    for (auto x = rect.topLeft.x; x < rect.topLeft.x + rect.size.x; ++x)
      for (auto y = rect.topLeft.y; y < rect.topLeft.y + rect.size.y; ++y)
        f(Point{x, y});
  }

  Rectangle intersection(const Rectangle& r1, const Rectangle& r2);

  struct AtBorder {
    AtBorder(const Rectangle& r, const Point& p)
      : top{p.y == r.topLeft.y},
        bottom{p.y == r.topLeft.y + r.size.y - 1},
        left{p.x == r.topLeft.x},
        right{p.x == r.topLeft.x + r.size.x - 1} {}

    bool top;
    bool bottom;
    bool left;
    bool right;
  };

  inline Rectangle boundingBox(const Rectangle& r) {
    return {r.topLeft - Vector{1, 1}, r.size + Vector{2, 2}};
  }

  Rectangle fixNegativeSize(Rectangle r);

  std::ostream& operator<<(std::ostream& os, const Point& p);
  std::ostream& operator<<(std::ostream& os, const Vector& v);
  std::ostream& operator<<(std::ostream& os, const Rectangle& r);
}

namespace std {
  template<>
  struct hash<util::geo::Point> {
    std::size_t operator()(const util::geo::Point& p) const noexcept {
      auto h1{std::hash<util::geo::Coordinate>{}(p.x)};
      auto h2{std::hash<util::geo::Coordinate>{}(p.y)};
      return h1 ^ (h2 << 1);
    }
  };
}
