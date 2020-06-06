#pragma once

#include "Resource.h"
#include "WorldObject.h"

#include <utility>

namespace rts {
  class ResourceField : public WorldObject {
  public:
    static constexpr Type worldObjectType = Type::ResourceField;

    ResourceCPtr resource() const { return resource_; }
    Quantity quantity() const { return quantity_; }

  protected:
    ResourceField(Point p, Vector s, ResourceCPtr r, Quantity q)
      : WorldObject{worldObjectType, p, s}, resource_{r}, quantity_{q} {}

  private:
    ResourceCPtr resource_;
    Quantity quantity_;
  };

  template<typename Derived, typename DerivedUi>
  class ResourceFieldTpl : public ResourceField {
  public:
    template<typename... UiArgs>
    ResourceFieldTpl(Point p, Vector s, ResourceCPtr r, Quantity q, UiArgs&&... uiArgs)
      : ResourceField{p, s, r, q}, ui_{std::forward<UiArgs>(uiArgs)...} {}

    template<typename... Args>
    static ResourceFieldSPtr create(Args&&... args) {
      return ResourceFieldSPtr{new Derived{std::forward<Args>(args)...}};
    }

    const DerivedUi& ui() const final { return ui_; }

  private:
    DerivedUi ui_;
  };
}
