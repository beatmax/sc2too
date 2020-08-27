#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace util {
  template<
      typename C,
      typename F,
      typename R = std::vector<std::invoke_result_t<F, typename C::value_type>>>
  R transform(const C& c, F f) {
    R r;
    r.reserve(c.size());
    std::transform(std::begin(c), std::end(c), std::back_inserter(r), f);
    return r;
  }

  template<typename C, typename P>
  C filter(const C& c, P p) {
    C r;
    std::copy_if(std::begin(c), std::end(c), std::back_inserter(r), p);
    return r;
  }

  template<typename C, typename V>
  bool contains(const C& c, const V& v) {
    return std::find(std::begin(c), std::end(c), v) != std::end(c);
  }
}
