#pragma once

#include "Ability.h"
#include "types.h"

#include <utility>

namespace rts {
  class Entity {
  public:
    virtual ~Entity() = 0;

    Position position;
    AbilityList abilities;

    static WorldActionList trigger(
        Ability& a, const World& world, const EntitySPtr& entity, Position target);
    static WorldActionList step(const World& world, const EntitySPtr& entity);
    void cancelAll();

    virtual const Ui& ui() const = 0;

  protected:
    Entity(Position p) : position{p} {}
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
