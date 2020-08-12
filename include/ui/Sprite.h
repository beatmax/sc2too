#pragma once

#include "rts/types.h"

#include <array>
#include <iosfwd>
#include <memory>
#include <vector>

namespace ui::detail {
  int defaultDefaultColor();
}

namespace ui {
  class SpriteMatrix;

  class Sprite {
  public:
    Sprite();
    explicit Sprite(const std::wstring& s);
    explicit Sprite(std::wistream&& is);
    explicit Sprite(std::vector<std::wstring>&& lines);
    ~Sprite();

    const SpriteMatrix& matrix() const { return *matrix_; }
    rts::Rectangle area(rts::Point topLeft) const;

  private:
    std::unique_ptr<SpriteMatrix> matrix_;
  };

  class Icon : public Sprite {
    using Sprite::Sprite;
  };

  template<typename T>
  class SpriteUi : public rts::Ui {
  public:
    virtual const Sprite& sprite(const T&) const = 0;
    virtual int defaultColor(const T&) const { return detail::defaultDefaultColor(); }
  };

  template<typename T>
  class IconUi : public rts::Ui {
  public:
    virtual const Icon& icon() const = 0;
  };

  template<typename T>
  inline const Sprite& getSprite(const T& object) {
    return static_cast<const SpriteUi<T>&>(*object.ui).sprite(object);
  }

  template<typename T>
  inline const Icon& getIcon(const T& object) {
    return static_cast<const IconUi<T>&>(*object.ui).icon();
  }

  template<typename T>
  inline int getDefaultColor(const T& object) {
    return static_cast<const SpriteUi<T>&>(*object.ui).defaultColor(object);
  }
}
