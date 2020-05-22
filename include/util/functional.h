#pragma once

#include <functional>

namespace util {

  template<typename T>
  struct Binder;

  template<typename C, typename... Args>
  struct Binder<void (C::*)(Args...)> {
    auto operator()(void (C::*func)(Args...), Args... args) const {
      using namespace std::placeholders;
      return std::bind(func, _1, args...);
    }
  };

}
