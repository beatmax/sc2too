#pragma once

#include "constants.h"
#include "types.h"
#include "util/WeakList.h"

#include <utility>
#include <vector>

namespace rts {
  class Group {
  public:
    void set(const World& w, UnitIdList ids);
    void add(const World& w, UnitIdList ids);
    void remove(const UnitIdList& ids);
    UnitIdList ids(const World& w) const;
    UnitCPtrList items(const World& w) const;
    bool contains(UnitId id) const { return list_.contains(id); }

  private:
    struct Compare {
      bool operator()(const Unit& u1, const Unit& u2) const;
    };

    util::WeakList<Unit, MaxSideUnits, Compare> list_;
  };
}
