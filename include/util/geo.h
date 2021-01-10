#pragma once

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <sys/types.h>
#include <tuple>
#include <vector>

namespace util::geo {
  using Coordinate = int32_t;

  class Cache;

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

  using PointList = std::vector<Point>;
  using VectorList = std::vector<Vector>;

  constexpr Point operator+(const Point& p, const Vector& v) { return {p.x + v.x, p.y + v.y}; }
  constexpr Point& operator+=(Point& p, const Vector& v) { return p = p + v; }
  constexpr Point operator-(const Point& p, const Vector& v) { return {p.x - v.x, p.y - v.y}; }
  constexpr Point& operator-=(Point& p, const Vector& v) { return p = p - v; }
  constexpr Vector operator-(const Point& a, const Point& b) { return {a.x - b.x, a.y - b.y}; }
  constexpr Vector operator+(const Vector& v1, const Vector& v2) {
    return {v1.x + v2.x, v1.y + v2.y};
  }
  constexpr Vector& operator+=(Vector& v1, const Vector& v2) { return v1 = v1 + v2; }
  constexpr Vector operator-(const Vector& v1, const Vector& v2) {
    return {v1.x - v2.x, v1.y - v2.y};
  }
  constexpr Vector& operator-=(Vector& v1, const Vector& v2) { return v1 = v1 - v2; }
  constexpr Vector operator-(const Vector& v) { return {-v.x, -v.y}; }
  constexpr Vector operator*(const Vector& v, Coordinate f) { return {v.x * f, v.y * f}; }
  constexpr Vector operator/(const Vector& v, Coordinate d) { return {v.x / d, v.y / d}; }
  PointList operator+(const Point& p, const VectorList& vs);

  constexpr Vector scale(const Vector& v, const Vector& s) { return {v.x * s.x, v.y * s.y}; }
  constexpr Vector scaleDiv(const Vector& v, const Vector& s) { return {v.x / s.x, v.y / s.y}; }

  constexpr float DirD{1.0};
  constexpr float DiagD{1.41};

  float diagonalDistance(Point a, Point b);

  struct Circle {
    Point center;
    Coordinate radius;
    Vector offset2{0, 0};

    PointList points(Cache& c) const;
  };

  struct Rectangle {
    Point topLeft;
    Vector size;

    Point bottomRight() const { return topLeft + size - Vector{1, 1}; }
    Point bottomRightOut() const { return topLeft + size; }
    Point center() const { return topLeft + (size - Vector{1, 1}) / 2; }

    bool operator==(const Rectangle& other) const {
      return topLeft == other.topLeft && size == other.size;
    }
    bool operator!=(const Rectangle& other) const { return !(*this == other); }

    bool contains(const Point& p) const {
      const auto& bro{bottomRightOut()};
      return p.x >= topLeft.x && p.y >= topLeft.y && p.x < bro.x && p.y < bro.y;
    }
    bool contains(const Rectangle& r) const {
      const auto& bro1{bottomRightOut()};
      const auto& bro2{r.bottomRightOut()};
      return topLeft.x <= r.topLeft.x && topLeft.y <= r.topLeft.y && bro1.x >= bro2.x &&
          bro1.y >= bro2.y;
    }

    struct PointIterator {
      using iterator_category = std::forward_iterator_tag;
      using value_type = Point;
      using difference_type = std::ptrdiff_t;
      using pointer = const Point*;
      using reference = const Point&;

      Point p;
      Coordinate left, right;

      Point operator*() const { return p; }
      const Point* operator->() const { return &p; }
      PointIterator& operator++() {
        if (++p.x == right) {
          p.x = left;
          ++p.y;
        }
        return *this;
      }
      bool operator==(const PointIterator& other) const { return p == other.p; }
      bool operator!=(const PointIterator& other) const { return p != other.p; }
    };

    struct OuterPointIterator {
      using iterator_category = std::forward_iterator_tag;
      using value_type = Point;
      using difference_type = void;
      using pointer = const Point*;
      using reference = const Point&;

      PointIterator pit;
      Coordinate top, bottom;

      Point operator*() const { return pit.p; }
      const Point* operator->() const { return &pit.p; }
      OuterPointIterator& operator++() {
        if (pit.p.x == pit.left && pit.p.y != top && pit.p.y != bottom &&
            (pit.right - pit.left) > 1)
          pit.p.x = pit.right - 1;
        else
          ++pit;
        return *this;
      }
      bool operator==(const OuterPointIterator& other) const { return pit == other.pit; }
      bool operator!=(const OuterPointIterator& other) const { return pit != other.pit; }
    };

    struct PointRange {
      explicit PointRange(const Rectangle& r)
        : b{r.topLeft, r.topLeft.x, r.topLeft.x + r.size.x},
          e{r.topLeft + Vector{0, r.size.y}, 0, 0} {}
      PointIterator b, e;
      PointIterator begin() const { return b; }
      PointIterator end() const { return e; }
    };

    struct OuterPointRange {
      explicit OuterPointRange(const Rectangle& r)
        : pr{r}, top{r.topLeft.y}, bottom{r.topLeft.y + r.size.y - 1} {}
      PointRange pr;
      Coordinate top, bottom;
      OuterPointIterator begin() const { return {pr.begin(), top, bottom}; }
      OuterPointIterator end() const { return {pr.end(), 0, 0}; }
    };

    PointRange points() const { return PointRange{*this}; }
    OuterPointRange outerPoints() const { return OuterPointRange{*this}; }
  };

  inline Rectangle rectangleCenteredAt(Point center, Vector size) {
    return Rectangle{center - (size - Vector{1, 1}) / 2, size};
  }
  Rectangle rectangleCenteredAt(Point center, Vector size, const Rectangle& bounds);
  Circle circleCenteredAt(const Rectangle& r, Coordinate radius);

  bool intersect(const Rectangle& r1, const Rectangle& r2);
  Rectangle intersection(const Rectangle& r1, const Rectangle& r2);
  std::optional<Rectangle> maybeIntersection(const Rectangle& r1, const Rectangle& r2);

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

  inline Rectangle boundingBox(const Point& p) { return {p - Vector{1, 1}, {3, 3}}; }
  inline Rectangle boundingBox(const Rectangle& r) {
    return {r.topLeft - Vector{1, 1}, r.size + Vector{2, 2}};
  }

  inline bool adjacent(const Point& p1, const Point& p2) {
    auto d{p1 - p2};
    return abs(d.x) <= 1 && abs(d.y) <= 1;
  }

  inline bool adjacent(const Rectangle& r1, const Rectangle& r2) {
    return intersect(r1, boundingBox(r2));
  }

  Rectangle fixNegativeSize(Rectangle r);

  class Cache {
  public:
    const VectorList& circle(Coordinate radius, Vector offset2);

  private:
    using CircleDesc = std::tuple<Coordinate, Coordinate, Coordinate>;

    std::map<CircleDesc, VectorList> circles_;
  };

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
