#pragma once

#include <functional>
#include <utility>

namespace util {

  template<typename T>
  struct Binder;

  // std::bind wrapper for member functions; the object argument is left free;
  // the other arguments are converted to the argument types of the given
  // function; no arguments are passed by reference (as with std::bind)
  template<typename C, typename R, typename... Args>
  struct Binder<R (C::*)(Args...)> {
    auto operator()(R (C::*func)(Args...), Args... args) const {
      using namespace std::placeholders;
      return std::bind(func, _1, std::forward<Args>(args)...);
    }
  };

}
