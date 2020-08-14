#pragma once

namespace rts {
  namespace detail {
    class SemaphoreAccess;
  }

  class Semaphore {
  public:
    explicit Semaphore(int v) : value_{v} {}
    bool blocked() const { return value_ == 0; }

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

  private:
    friend class detail::SemaphoreAccess;
    int value_;
  };
}
