#pragma once

#include "types.h"

namespace rts {
  class Entity {
  public:
    explicit Entity(Position p) : position{p} {}
    virtual ~Entity() = 0;

    Position position;
  };

  template<typename Derived>
  class EntityTpl : public Entity {
  public:
    using Entity::Entity;

    template<typename... Args>
    static EntitySPtr create(Args&&... args) {
      return EntitySPtr{new Derived{std::forward<Args>(args)...}};
    }
  };
}
