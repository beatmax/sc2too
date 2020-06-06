#pragma once

#include "types.h"

namespace rts {
  struct WorldObject {
    enum class Type { Blocker, Entity, ResourceField };

    WorldObject(Type t, Point p, Vector s) : type{t}, area{p, s} {}
    virtual ~WorldObject() = 0;

    const Type type;
    Rectangle area;

    virtual const Ui& ui() const = 0;
  };
}
