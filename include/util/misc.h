#pragma once

namespace util {

  template<typename T>
  inline T unit(T v) {
    return v > 0 ? 1 : v < 0 ? -1 : 0;
  }
}
