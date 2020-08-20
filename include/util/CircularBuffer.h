#pragma once

#include <array>
#include <cassert>
#include <utility>

namespace util {
  template<typename T, size_t N>
  class CircularBuffer {
  public:
    T& push();
    void push(T t);
    void pop();
    const T& front() const;
    size_t size() const;
    bool empty() const { return first_ == last_; }
    const T& operator[](size_t i) const;

  private:
    static constexpr size_t Sz{N + 1};

    using Buffer = std::array<T, Sz>;

    Buffer buffer_;
    size_t first_{0};
    size_t last_{0};
  };
}

template<typename T, size_t N>
T& util::CircularBuffer<T, N>::push() {
  T& ref{buffer_[last_]};
  last_ = (last_ + 1) % Sz;
  if (last_ == first_)
    first_ = (first_ + 1) % Sz;
  return ref;
}

template<typename T, size_t N>
inline void util::CircularBuffer<T, N>::push(T t) {
  push() = std::move(t);
}

template<typename T, size_t N>
inline void util::CircularBuffer<T, N>::pop() {
  assert(!empty());
  first_ = (first_ + 1) % Sz;
}

template<typename T, size_t N>
inline const T& util::CircularBuffer<T, N>::front() const {
  assert(!empty());
  return buffer_[first_];
}

template<typename T, size_t N>
inline size_t util::CircularBuffer<T, N>::size() const {
  auto lst{(last_ < first_) ? last_ + Sz : last_};
  return lst - first_;
}

template<typename T, size_t N>
inline const T& util::CircularBuffer<T, N>::operator[](size_t i) const {
  assert(i < size());
  return buffer_[(first_ + i) % Sz];
}
