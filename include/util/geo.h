#pragma once

#include <algorithm>
#include <cmath>
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

    bool operator==(Point other) const { return x == other.x && y == other.y; }
    bool operator!=(Point other) const { return !(*this == other); }
  };

  struct Vector {
    Coordinate x;
    Coordinate y;

    bool operator==(Vector other) const { return x == other.x && y == other.y; }
    bool operator!=(Vector other) const { return !(*this == other); }
  };

  struct FVector {
    float x;
    float y;
  };

  using PointList = std::vector<Point>;
  using VectorList = std::vector<Vector>;

  constexpr Point toPoint(Vector v) { return Point{v.x, v.y}; }
  constexpr Vector toVector(Point p) { return Vector{p.x, p.y}; }
  constexpr Vector round(FVector v) { return {Coordinate(lroundf(v.x)), Coordinate(lroundf(v.y))}; }
  constexpr FVector toFVector(Vector v) { return {float(v.x), float(v.y)}; }

  constexpr Coordinate unit(Coordinate c) { return c > 0 ? 1 : c < 0 ? -1 : 0; }
  constexpr Vector unit(Vector v) { return {unit(v.x), unit(v.y)}; }

  constexpr Point operator+(Point p, Vector v) { return {p.x + v.x, p.y + v.y}; }
  constexpr Point& operator+=(Point& p, Vector v) { return p = p + v; }
  constexpr Point operator-(Point p, Vector v) { return {p.x - v.x, p.y - v.y}; }
  constexpr FVector operator-(Point p, FVector v) { return {float(p.x) - v.x, float(p.y) - v.y}; }
  constexpr Vector operator-(Point a, Point b) { return {a.x - b.x, a.y - b.y}; }
  constexpr Point& operator-=(Point& p, Vector v) { return p = p - v; }

  constexpr Vector operator+(Vector v1, Vector v2) { return {v1.x + v2.x, v1.y + v2.y}; }
  constexpr FVector operator+(FVector v1, FVector v2) { return {v1.x + v2.x, v1.y + v2.y}; }
  constexpr FVector operator+(Vector v1, FVector v2) { return toFVector(v1) + v2; }
  constexpr Vector& operator+=(Vector& v1, Vector v2) { return v1 = v1 + v2; }
  constexpr Vector operator-(Vector v1, Vector v2) { return {v1.x - v2.x, v1.y - v2.y}; }
  constexpr FVector operator-(FVector v1, FVector v2) { return {v1.x - v2.x, v1.y - v2.y}; }
  constexpr FVector operator-(Vector v1, FVector v2) { return toFVector(v1) - v2; }
  constexpr Vector& operator-=(Vector& v1, Vector v2) { return v1 = v1 - v2; }
  constexpr Vector operator-(Vector v) { return {-v.x, -v.y}; }

  constexpr Vector operator*(Vector v, Coordinate f) { return {v.x * f, v.y * f}; }
  constexpr Vector operator/(Vector v, Coordinate d) { return {v.x / d, v.y / d}; }
  constexpr Vector operator*(Coordinate f, Vector v) { return {f * v.x, f * v.y}; }
  constexpr Vector operator/(Coordinate n, Vector v) { return {n / v.x, n / v.y}; }

  constexpr FVector operator*(FVector v, float f) { return {v.x * f, v.y * f}; }
  constexpr FVector operator/(FVector v, float d) { return {v.x / d, v.y / d}; }
  constexpr FVector operator*(float f, FVector v) { return {f * v.x, f * v.y}; }
  constexpr FVector operator/(float n, FVector v) { return {n / v.x, n / v.y}; }

  PointList operator+(Point p, const VectorList& vs);

  inline Vector min(Vector v1, Vector v2) { return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)}; }
  inline Vector max(Vector v1, Vector v2) { return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)}; }

  constexpr FVector relation(Vector v1, Vector v2) {
    return {float(v1.x) / float(v2.x), float(v1.y) / float(v2.y)};
  }

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

    bool contains(Point p) const {
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

  Point clamp(Point p, const Rectangle& r);
  bool intersect(const Rectangle& r1, const Rectangle& r2);
  Rectangle intersection(const Rectangle& r1, const Rectangle& r2);
  std::optional<Rectangle> maybeIntersection(const Rectangle& r1, const Rectangle& r2);

  struct AtBorder {
    AtBorder(const Rectangle& r, Point p)
      : top{p.y == r.topLeft.y},
        bottom{p.y == r.topLeft.y + r.size.y - 1},
        left{p.x == r.topLeft.x},
        right{p.x == r.topLeft.x + r.size.x - 1} {}

    bool top;
    bool bottom;
    bool left;
    bool right;
  };

  inline Rectangle boundingBox(Point p) { return {p - Vector{1, 1}, {3, 3}}; }
  inline Rectangle boundingBox(const Rectangle& r) {
    return {r.topLeft - Vector{1, 1}, r.size + Vector{2, 2}};
  }

  inline bool adjacent(Point p1, Point p2) {
    auto d{p1 - p2};
    return abs(d.x) <= 1 && abs(d.y) <= 1;
  }

  inline bool adjacent(const Rectangle& r1, const Rectangle& r2) {
    return intersect(r1, boundingBox(r2));
  }

  Rectangle fixNegativeSize(Rectangle r);

  constexpr Vector scale(Vector v, Vector s) { return {v.x * s.x, v.y * s.y}; }
  constexpr Vector scale(Vector v, FVector s) {
    return {Coordinate(float(v.x) * s.x), Coordinate(float(v.y) * s.y)};
  }
  constexpr FVector scale(FVector v, FVector s) { return {v.x * s.x, v.y * s.y}; }
  constexpr FVector fscale(Vector v, FVector s) { return {float(v.x) * s.x, float(v.y) * s.y}; }
  constexpr Vector scaleDiv(Vector v, Vector s) { return {v.x / s.x, v.y / s.y}; }

  template<typename V>
  constexpr Point transform(Point p, V s, Point osrc, Point dsrc) {
    return dsrc + scale(p - osrc, s);
  }
  template<typename V>
  constexpr Point transform(Point p, V s) {
    return transform(p, s, {0, 0}, {0, 0});
  }
  template<typename V>
  constexpr Rectangle transform(const Rectangle& r, V s, Point osrc, Point dsrc) {
    return {transform(r.topLeft, s, osrc, dsrc), scale(r.size, s)};
  }
  template<typename V>
  constexpr Rectangle transform(const Rectangle& r, V s) {
    return transform(r, s, {0, 0}, {0, 0});
  }
  constexpr Point transformDiv(Point p, Vector s, Point osrc, Point dsrc) {
    return dsrc + scaleDiv(p - osrc, s);
  }

  class Cache {
  public:
    const VectorList& circle(Coordinate radius, Vector offset2);

  private:
    using CircleDesc = std::tuple<Coordinate, Coordinate, Coordinate>;

    std::map<CircleDesc, VectorList> circles_;
  };

  std::ostream& operator<<(std::ostream& os, Point p);
  std::ostream& operator<<(std::ostream& os, Vector v);
  std::ostream& operator<<(std::ostream& os, FVector v);
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
