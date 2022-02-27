#pragma once

#include "rts/WorldObject.h"
#include "rts/types.h"
#include "types.h"
#include "util/DynValue.h"

#include <array>
#include <iosfwd>
#include <vector>

namespace ui::detail {
  Color defaultDefaultColor();
}

namespace ui {
  class Frame;

  class Sprite {
  public:
    Sprite();
    explicit Sprite(const std::wstring& s);
    explicit Sprite(std::wistream&& is);
    explicit Sprite(std::vector<std::wstring>&& lines);
    explicit Sprite(util::DynValue<Frame> frame);
    ~Sprite();

    const Frame& frame(FrameIndex frame = 0, rts::Direction direction = rts::Direction(0)) const;
    FrameIndex frameCount() const { return frameCount_; }
    rts::GameTime frameDuration() const { return frameDuration_; }
    bool directional() const { return directional_; }
    rts::Rectangle area(rts::Point topLeft) const;

  private:
    std::vector<util::DynValue<Frame>> frames_;
    FrameIndex frameCount_;
    rts::GameTime frameDuration_{0};
    bool directional_{false};
  };

  class Icon : public Sprite {
    using Sprite::Sprite;
  };

  class SpriteState {
  public:
    void update(const rts::World& w, const Sprite* s);

    operator bool() const { return sprite_; }
    const Sprite& sprite() const { return *sprite_; }
    const Frame& frame(rts::Direction direction) const {
      return sprite_->frame(frameIndex_, direction);
    }

  private:
    const Sprite* sprite_{};
    FrameIndex frameIndex_{};
    rts::GameTime nextFrameTime_{};
  };

  class SpriteUiBase : public rts::Ui {
  public:
    void update(const rts::World& w, const rts::WorldObject& obj);

    const SpriteState& base() const { return base_; }
    const SpriteState& overlay() const { return overlay_; }
    virtual Color defaultColorPublic(const rts::WorldObject&) const = 0;

  protected:
    virtual const Sprite& spriteProtected(const rts::World&, const rts::WorldObject&) const = 0;
    virtual const Sprite* overlayProtected(const rts::World&, const rts::WorldObject&) const = 0;

  private:
    SpriteState base_;
    SpriteState overlay_;
  };

  class SpriteUiFacade {
  public:
    explicit SpriteUiFacade(SpriteUiBase& ui, const rts::World& w, const rts::WorldObject& obj)
      : ui_{ui} {
      ui_.update(w, obj);
    }
    const SpriteState& base() const { return ui_.base(); }
    const SpriteState& overlay() const { return ui_.overlay(); }
    Color defaultColor(const rts::WorldObject& obj) const { return ui_.defaultColorPublic(obj); }

  private:
    SpriteUiBase& ui_;
  };

  template<typename T>
  class SpriteUi : public SpriteUiBase {
  public:
    const Sprite& spriteProtected(const rts::World& w, const rts::WorldObject& obj) const final {
      return sprite(w, rts::StableRef<T>{static_cast<const T&>(obj)});
    }
    const Sprite* overlayProtected(const rts::World& w, const rts::WorldObject& obj) const final {
      return overlay(w, rts::StableRef<T>{static_cast<const T&>(obj)});
    }
    Color defaultColorPublic(const rts::WorldObject& obj) const final {
      return defaultColor(rts::StableRef<T>{static_cast<const T&>(obj)});
    }

  private:
    virtual const Sprite& sprite(const rts::World& w, rts::StableRef<T>) const = 0;
    virtual const Sprite* overlay(const rts::World& w, rts::StableRef<T>) const { return nullptr; }
    virtual Color defaultColor(rts::StableRef<T>) const { return detail::defaultDefaultColor(); }
  };

  template<typename T>
  class IconUi : public rts::Ui {
  public:
    virtual const Icon& icon() const = 0;
  };

  inline SpriteUiFacade getUpdatedSpriteUi(const rts::World& w, const rts::WorldObject& obj) {
    return SpriteUiFacade{static_cast<SpriteUiBase&>(*obj.ui), w, obj};
  }

  template<typename T>
  inline const Icon& getIcon(const T& obj) {
    return static_cast<const IconUi<T>&>(*obj.ui).icon();
  }
}
