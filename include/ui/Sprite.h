#pragma once

#include "rts/types.h"
#include "util/Matrix.h"

#include <iosfwd>
#include <ncurses.h>
#include <string>
#include <vector>

namespace ui {
  struct Sprite {
    util::Matrix<chtype> matrix;

    Sprite() = default;
    explicit Sprite(std::initializer_list<chtype> list) : matrix(list) {}
    explicit Sprite(const std::string& s);
    explicit Sprite(std::istream&& is);
    explicit Sprite(const std::vector<std::string>& lines);
  };

  template<typename T>
  class SpriteUi : public rts::Ui {
  public:
    virtual const Sprite& sprite(const T&) const = 0;
  };

  template<typename T>
  inline const Sprite& getSprite(const T& object) {
    return static_cast<const SpriteUi<T>&>(object.ui()).sprite(object);
  }
}
