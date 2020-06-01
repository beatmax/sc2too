#pragma once

#include "Ability.h"
#include "types.h"

#include <utility>

namespace rts {
  class Entity {
  public:
    virtual ~Entity() = 0;

    Rectangle area;
    AbilityList abilities;

    static WorldActionList trigger(
        Ability& a, const World& world, const EntitySPtr& entity, Point target);
    static WorldActionList step(const World& world, const EntitySPtr& entity);
    void cancelAll();

    virtual const Ui& ui() const = 0;

  protected:
    Entity(Point p, Vector s) : area{p, s} {}
    void addAbility(Ability&& a) { abilities.push_back(std::move(a)); }
  };

  template<typename Derived, typename DerivedUi>
  class EntityTpl : public Entity {
  public:
    using Entity::Entity;

    template<typename... Args>
    static EntitySPtr create(Args&&... args) {
      return EntitySPtr{new Derived{std::forward<Args>(args)...}};
    }

    const DerivedUi& ui() const final { return ui_; }

  private:
    DerivedUi ui_;
  };
}
