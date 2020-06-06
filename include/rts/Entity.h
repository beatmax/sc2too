#pragma once

#include "Ability.h"
#include "WorldObject.h"

#include <utility>

namespace rts {
  class Entity : public WorldObject {
  public:
    static constexpr Type worldObjectType = Type::Entity;

    AbilityList abilities;

    static WorldActionList trigger(
        Ability& a, const World& world, const EntitySPtr& entity, Point target);
    static WorldActionList step(const World& world, const EntitySPtr& entity);
    void cancelAll();

  protected:
    Entity(Point p, Vector s) : WorldObject{worldObjectType, p, s} {}
    void addAbility(Ability&& a) { abilities.push_back(std::move(a)); }
  };

  template<typename Derived, typename DerivedUi>
  class EntityTpl : public Entity {
  public:
    template<typename... UiArgs>
    EntityTpl(Point p, Vector s, UiArgs&&... uiArgs)
      : Entity{p, s}, ui_{std::forward<UiArgs>(uiArgs)...} {}

    template<typename... Args>
    static EntitySPtr create(Args&&... args) {
      return EntitySPtr{new Derived{std::forward<Args>(args)...}};
    }

    const DerivedUi& ui() const final { return ui_; }

  private:
    DerivedUi ui_;
  };
}
