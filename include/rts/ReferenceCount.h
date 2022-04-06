#pragma once

namespace rts {
  namespace detail {
    class ReferenceCountAccess;
  }

  class ReferenceCount {
  public:
    int value() const { return value_; }

    ReferenceCount() = default;
    ReferenceCount(const ReferenceCount&) = delete;
    ReferenceCount& operator=(const ReferenceCount&) = delete;

  private:
    friend class detail::ReferenceCountAccess;
    int value_{0};
  };
}
