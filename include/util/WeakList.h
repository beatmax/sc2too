#pragma once

#include "Pool.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <vector>

namespace util {

  template<typename T, uint32_t N, typename Compare = std::less<T>>
  class WeakList {
  public:
    using Value = T;
    using Id = PoolObjectId<Value>;
    using WeakId = PoolObjectWeakId<Value>;
    using Array = std::array<WeakId, N>;
    using IdList = std::vector<Id>;

    explicit WeakList(const Compare& comp = Compare{}) : comp_{comp} {}

    template<typename Pool>
    WeakList(const Pool& pool, const IdList& ids, const Compare& comp = Compare{})
      : WeakList{comp} {
      set(pool, ids);
    }

    template<typename Pool>
    void set(const Pool& pool, IdList ids);

    template<typename Pool>
    void add(const Pool& pool, IdList ids);

    void remove(const IdList& ids);

    template<typename Pool>
    IdList lock(Pool& pool);

    template<typename Pool>
    auto items(Pool& pool) {
      return pool[lock(pool)];
    }

  private:
    template<typename Pool>
    void doSet(const Pool& pool, const IdList& ids);

    Compare comp_;
    Array array_;
    uint32_t n_{0};
  };

  template<typename T, uint32_t N, typename Compare>
  template<typename Pool>
  void WeakList<T, N, Compare>::set(const Pool& pool, IdList ids) {
    PoolCompare poolComp{pool, comp_};
    std::sort(ids.begin(), ids.end(), poolComp);
    doSet(pool, ids);
  }

  template<typename T, uint32_t N, typename Compare>
  template<typename Pool>
  void WeakList<T, N, Compare>::add(const Pool& pool, IdList ids) {
    PoolCompare poolComp{pool, comp_};
    std::sort(ids.begin(), ids.end(), poolComp);
    auto currentIds{lock(pool)};
    IdList updated;
    updated.reserve(currentIds.size() + ids.size());
    std::set_union(
        currentIds.begin(), currentIds.end(), ids.begin(), ids.end(), std::back_inserter(updated),
        poolComp);
    doSet(pool, updated);
  }

  template<typename T, uint32_t N, typename Compare>
  void WeakList<T, N, Compare>::remove(const IdList& ids) {
    auto it = std::remove_if(array_.begin(), array_.end(), [&ids](WeakId wid) {
      return std::find(ids.begin(), ids.end(), Id(wid)) != ids.end();
    });
    n_ = std::distance(array_.begin(), it);
  }

  template<typename T, uint32_t N, typename Compare>
  template<typename Pool>
  auto WeakList<T, N, Compare>::lock(Pool& pool) -> IdList {
    Array updated;
    IdList ids;
    ids.reserve(n_);
    for (uint32_t i = 0; i < n_; ++i) {
      auto wid{array_[i]};
      if (auto id = wid.lock(pool)) {
        updated[ids.size()] = wid;
        ids.push_back(id);
      }
    }
    if (n_ != ids.size()) {
      n_ = ids.size();
      array_ = updated;
    }
    return ids;
  }

  template<typename T, uint32_t N, typename Compare>
  template<typename Pool>
  void WeakList<T, N, Compare>::doSet(const Pool& pool, const IdList& ids) {
    assert(ids.size() <= N);
    for (uint32_t i = 0; i < ids.size(); ++i)
      array_[i] = WeakId{pool, ids[i]};
    n_ = ids.size();
  }

}
