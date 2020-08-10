#pragma once

#include "constants.h"
#include "types.h"

#include <variant>

namespace rts {
  struct Cell {
    using Empty = std::monostate;
    using Content = std::variant<Empty, BlockerId, EntityId, ResourceFieldId>;
    enum ContentType { Nothing, Blocker, Entity, ResourceField };

    Content content;

    ContentType contentType() const { return static_cast<ContentType>(content.index()); }
    bool contains(ContentType ct) const { return contentType() == ct; }
    bool empty() const { return contains(Nothing); }

    BlockerId blockerId() const { return getId<BlockerId>(); }
    EntityId entityId() const { return getId<EntityId>(); }
    ResourceFieldId resourceFieldId() const { return getId<ResourceFieldId>(); }

    template<typename T>
    T getId() const {
      if (auto* p{std::get_if<T>(&content)})
        return *p;
      else
        return {};
    }

#ifdef MAP_DEBUG
    struct {
      int color{0};
    } debug;
#endif
  };

}
