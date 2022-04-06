#pragma once

#include "ReferenceCount.h"
#include "World.h"

#include <cassert>
#include <utility>

namespace rts {
  namespace detail {
    struct ReferenceCountAccess {
      static void inc(ReferenceCount& rc) { ++rc.value_; }
      static void dec(ReferenceCount& rc) { --rc.value_; }
    };
  }

  template<typename T, ReferenceCount T::*P>
  class CountedReference {
  public:
    using WeakId = util::PoolObjectWeakId<T>;

    CountedReference() = default;
    CountedReference(World& w, WeakId wid);
    ~CountedReference();

    CountedReference(const CountedReference&) = delete;
    CountedReference& operator=(const CountedReference&) = delete;
    CountedReference(CountedReference&& other) { std::swap(wid_, other.wid_); }
    CountedReference& operator=(CountedReference&& other) {
      std::swap(wid_, other.wid_);
      return *this;
    }

    WeakId weakId() const { return wid_; }
    void release(World& w);
    WeakId detach() { return std::exchange(wid_, {}); }
    operator bool() const { return bool{wid_}; }

  private:
    WeakId wid_;
  };
}

template<typename T, rts::ReferenceCount T::*P>
inline rts::CountedReference<T, P>::CountedReference(World& w, WeakId wid) {
  if (auto* obj{w[wid]}) {
    detail::ReferenceCountAccess::inc(obj->*P);
    wid_ = wid;
  }
}

template<typename T, rts::ReferenceCount T::*P>
rts::CountedReference<T, P>::~CountedReference() {
  assert(!wid_);
}

template<typename T, rts::ReferenceCount T::*P>
void rts::CountedReference<T, P>::release(World& w) {
  if (wid_) {
    if (auto* obj{w[wid_]})
      detail::ReferenceCountAccess::dec(obj->*P);
    wid_ = WeakId{};
  }
}
