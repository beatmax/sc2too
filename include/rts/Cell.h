#pragma once

#include "constants.h"
#include "types.h"

#include <variant>

namespace rts {
  struct Cell {
    using Empty = std::monostate;
    using Content = std::variant<Empty, BlockerId, UnitId, ResourceFieldId>;
    enum ContentType { Nothing, Blocker, Unit, ResourceField };

    Content content;
    MapSegmentId segment{0};
    MinimapCellId minimapCell{0};

    ContentType contentType() const { return static_cast<ContentType>(content.index()); }
    bool contains(ContentType ct) const { return contentType() == ct; }
    bool empty() const { return contains(Nothing); }

    BlockerId blockerId() const { return getId<BlockerId>(); }
    UnitId unitId() const { return getId<UnitId>(); }
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
      bool highlight{false};
    } debug;
#endif
  };

}
