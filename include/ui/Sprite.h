#pragma once

#include "rts/types.h"

#include <array>
#include <iosfwd>
#include <memory>
#include <vector>

namespace ui {
  class SpriteMatrix;

  class Sprite {
  public:
    Sprite();
    explicit Sprite(const std::wstring& s);
    explicit Sprite(std::wistream&& is);
    explicit Sprite(const std::vector<std::wstring>& lines);
    ~Sprite();

    const SpriteMatrix& matrix() const { return *matrix_; }
    rts::Rectangle area(rts::Point topLeft) const;

  private:
    std::unique_ptr<SpriteMatrix> matrix_;
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
