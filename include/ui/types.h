#pragma once

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "util/Matrix.h"
#include "util/geo.h"

#include <cstdint>

namespace ui {
  class Frame;
  class Sprite;

  using FrameIndex = uint32_t;

  using MatrixCoordinate = util::geo::Coordinate;
  using MatrixPoint = util::geo::Point;
  using MatrixVector = util::geo::Vector;
  using MatrixRect = util::geo::Rectangle;

  using ScreenCoordinate = util::geo::Coordinate;
  using ScreenPoint = util::geo::Point;
  using ScreenVector = util::geo::Vector;
  using ScreenRect = util::geo::Rectangle;

  using PixelCoordinate = util::geo::Coordinate;
  using PixelPoint = util::geo::Point;
  using PixelVector = util::geo::Vector;
  using PixelRect = util::geo::Rectangle;

  using FVector = util::geo::FVector;

  struct SubcharPoint {
    ScreenPoint point;
    FVector subchar;
  };

  template<typename T>
  class Matrix : public util::Matrix<T, MatrixCoordinate> {
    using Inherited = util::Matrix<T, MatrixCoordinate>;

  public:
    explicit Matrix(Inherited m) : Inherited{std::move(m)} {}
    explicit Matrix(MatrixVector sz) : Inherited{sz.y, sz.x} {}
    using Inherited::cols;
    using Inherited::rows;
    T& operator[](MatrixPoint p) { return (*this)(p.y, p.x); }
    const T& operator[](MatrixPoint p) const { return (*this)(p.y, p.x); }
    MatrixRect rect() const { return {{0, 0}, {cols(), rows()}}; }
  };

  struct Window {
    WINDOW* w{};
    int maxY{}, maxX{};
    int beginY{}, beginX{};
  };

  enum class Color : int {
    Red = 1,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Gray,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite,
    Black,
    DarkBlue,
    DarkGreen = 22,
    DarkRed = 52,
    DarkGray = 237,
    ElectricBlue1 = 27,
    ElectricBlue2 = 33,
    Default = -1
  };

  using ScrollDirection = uint32_t;
  constexpr ScrollDirection ScrollDirectionLeft{0b0001};
  constexpr ScrollDirection ScrollDirectionRight{0b0010};
  constexpr ScrollDirection ScrollDirectionUp{0b0100};
  constexpr ScrollDirection ScrollDirectionDown{0b1000};
}
