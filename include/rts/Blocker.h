#pragma once

#include "WorldObject.h"

#include <utility>

namespace rts {
  class Blocker : public WorldObject {
  public:
    static constexpr Type worldObjectType = Type::Blocker;

  protected:
    Blocker(Point p, Vector s) : WorldObject{worldObjectType, p, s} {}
  };

  template<typename Derived, typename DerivedUi>
  class BlockerTpl : public Blocker {
  public:
    template<typename... UiArgs>
    BlockerTpl(Point p, Vector s, UiArgs&&... uiArgs)
      : Blocker{p, s}, ui_{std::forward<UiArgs>(uiArgs)...} {}

    template<typename... Args>
    static BlockerSPtr create(Args&&... args) {
      return BlockerSPtr{new Derived{std::forward<Args>(args)...}};
    }

    const DerivedUi& ui() const final { return ui_; }

  private:
    DerivedUi ui_;
  };
}
