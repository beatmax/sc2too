#pragma once

#include "rts/WorldObject.h"
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

  class SpriteUiBase : public rts::Ui {
  public:
    virtual const Sprite& spriteBase(const rts::World& w, const rts::WorldObject&) const = 0;
    virtual int defaultColorBase(const rts::WorldObject&) const = 0;
  };

  template<typename T>
  class SpriteUi : public SpriteUiBase {
  public:
    const Sprite& spriteBase(const rts::World& w, const rts::WorldObject& object) const final {
      return sprite(w, rts::StableRef<T>{static_cast<const T&>(object)});
    }
    int defaultColorBase(const rts::WorldObject& object) const final {
      return defaultColor(rts::StableRef<T>{static_cast<const T&>(object)});
    }

  private:
    virtual const Sprite& sprite(const rts::World& w, rts::StableRef<T>) const = 0;
    virtual int defaultColor(rts::StableRef<T>) const { return detail::defaultDefaultColor(); }
  };

  template<typename T>
  class IconUi : public rts::Ui {
  public:
    virtual const Icon& icon() const = 0;
  };

  inline const Sprite& getSprite(const rts::World& w, const rts::WorldObject& object) {
    return static_cast<const SpriteUiBase&>(*object.ui).spriteBase(w, object);
  }

  inline int getDefaultColor(const rts::WorldObject& object) {
    return static_cast<const SpriteUiBase&>(*object.ui).defaultColorBase(object);
  }

  template<typename T>
  inline const Icon& getIcon(const T& object) {
    return static_cast<const IconUi<T>&>(*object.ui).icon();
  }
}
