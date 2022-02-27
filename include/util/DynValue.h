#pragma once

#include <memory>
#include <utility>

namespace util {
  template<typename T>
  class DynValue {
  public:
    DynValue() : p_{std::make_unique<T>()} {}
    DynValue(const T& v) : p_{std::make_unique<T>(v)} {}
    DynValue(T&& v) noexcept : p_{std::make_unique<T>(std::move(v))} {
      static_assert(noexcept(T(std::declval<T>())));
    }
    DynValue(const DynValue& other) : DynValue{*other} {}
    DynValue(DynValue&& other) noexcept : DynValue{std::move(*other)} {}

    DynValue& operator=(const T& v) { return *p_ = v, *this; }
    DynValue& operator=(T&& v) noexcept {
      static_assert(noexcept(std::declval<T>() = std::declval<T>()));
      return *p_ = std::move(v), *this;
    }
    DynValue& operator=(const DynValue& other) { return *p_ = *other, *this; }
    DynValue& operator=(DynValue&& other) noexcept {
      static_assert(noexcept(std::declval<T>() = std::declval<T>()));
      return *p_ = std::move(*other), *this;
    }

    T& operator*() { return *p_; }
    const T& operator*() const { return *p_; }
    T* operator->() { return &*p_; }
    const T* operator->() const { return &*p_; }

    operator T&() { return *p_; }
    operator const T&() const { return *p_; }

    friend bool operator==(const DynValue& x, const DynValue& y) { return *x == *y; }
    friend bool operator==(const T& x, const DynValue& y) { return x == *y; }
    friend bool operator==(const DynValue& x, const T& y) { return *x == y; }

    friend bool operator!=(const DynValue& x, const DynValue& y) { return *x != *y; }
    friend bool operator!=(const T& x, const DynValue& y) { return x != *y; }
    friend bool operator!=(const DynValue& x, const T& y) { return *x != y; }

    friend bool operator<(const DynValue& x, const DynValue& y) { return *x < *y; }
    friend bool operator<(const T& x, const DynValue& y) { return x < *y; }
    friend bool operator<(const DynValue& x, const T& y) { return *x < y; }

    friend bool operator<=(const DynValue& x, const DynValue& y) { return *x <= *y; }
    friend bool operator<=(const T& x, const DynValue& y) { return x <= *y; }
    friend bool operator<=(const DynValue& x, const T& y) { return *x <= y; }

    friend bool operator>(const DynValue& x, const DynValue& y) { return *x > *y; }
    friend bool operator>(const T& x, const DynValue& y) { return x > *y; }
    friend bool operator>(const DynValue& x, const T& y) { return *x > y; }

    friend bool operator>=(const DynValue& x, const DynValue& y) { return *x >= *y; }
    friend bool operator>=(const T& x, const DynValue& y) { return x >= *y; }
    friend bool operator>=(const DynValue& x, const T& y) { return *x >= y; }

  private:
    std::unique_ptr<T> p_;
  };
}
