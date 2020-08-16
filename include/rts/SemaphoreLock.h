#pragma once

#include "Semaphore.h"
#include "World.h"

#include <cassert>
#include <utility>

namespace rts {
  namespace detail {
    struct SemaphoreAccess {
      static void inc(Semaphore& s) { ++s.value_; }
      static bool dec(Semaphore& s) {
        bool b{s.value_ > 0};
        if (b)
          --s.value_;
        return b;
      }
    };
  }

  template<typename T>
  class SemaphoreLock {
  public:
    using WeakId = util::PoolObjectWeakId<T>;

    SemaphoreLock() = default;
    SemaphoreLock(World& w, WeakId wid);
    ~SemaphoreLock();

    SemaphoreLock(const SemaphoreLock&) = delete;
    SemaphoreLock& operator=(const SemaphoreLock&) = delete;
    SemaphoreLock(SemaphoreLock&& other) { std::swap(wid_, other.wid_); }
    SemaphoreLock& operator=(SemaphoreLock&& other) {
      std::swap(wid_, other.wid_);
      return *this;
    }

    void release(World& w);
    WeakId detach() { return std::exchange(wid_, {}); }
    operator bool() const { return bool{wid_}; }

  private:
    WeakId wid_;
  };
}

template<typename T>
inline rts::SemaphoreLock<T>::SemaphoreLock(World& w, WeakId wid) : wid_{wid} {
  if (auto* obj{w[wid_]}) {
    if (detail::SemaphoreAccess::dec(obj->sem))
      return;
  }
  wid_ = WeakId{};
}

template<typename T>
rts::SemaphoreLock<T>::~SemaphoreLock() {
  assert(!wid_);
}

template<typename T>
void rts::SemaphoreLock<T>::release(World& w) {
  if (wid_) {
    if (auto* obj{w[wid_]})
      detail::SemaphoreAccess::inc(obj->sem);
    wid_ = WeakId{};
  }
}
