#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
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

  template<typename C, typename P>
  bool allOf(const C& c, P p) {
    return std::all_of(std::begin(c), std::end(c), p);
  }

  template<typename C, typename P>
  auto findIf(const C& c, P p) -> decltype(std::begin(c)) {
    return std::find_if(std::begin(c), std::end(c), p);
  }

  template<typename C, typename P>
  auto findIfOpt(const C& c, P p) -> std::optional<decltype(*std::begin(c))> {
    if (auto it{findIf(c, p)}; it != std::end(c))
      return *it;
    return std::nullopt;
  }

  template<typename C, typename F>
  auto minElementBy(const C& c, F f) -> decltype(std::begin(c)) {
    auto it{std::begin(c)};
    auto last{std::end(c)};
    if (it == last)
      return last;

    auto smallest{it};
    auto smallestValue{f(*it)};
    ++it;
    for (; it != last; ++it) {
      auto value{f(*it)};
      if (value < smallestValue) {
        smallest = it;
        smallestValue = value;
      }
    }
    return smallest;
  }
}
