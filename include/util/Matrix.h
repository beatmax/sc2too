#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

namespace util {

  template<typename T, typename Dim = size_t>
  class Matrix {
  public:
    Matrix() : rows_{0}, cols_{0} {}
    Matrix(Dim rows, Dim cols) : rows_{rows}, cols_{cols}, data_(rows * cols) {}
    Matrix(std::initializer_list<T> list) : rows_{1}, cols_{list.size()}, data_(list) {}

    Dim rows() const { return rows_; }
    Dim cols() const { return cols_; }
    size_t size() const { return data_.size(); }

    T& operator()(Dim i, Dim j) {
      assert(i >= 0 && j >= 0 && i < rows_ && j < cols_);
      return data_[i * cols_ + j];
    }
    const T& operator()(Dim i, Dim j) const {
      assert(i >= 0 && j >= 0 && i < rows_ && j < cols_);
      return data_[i * cols_ + j];
    }

    void setRaw(const std::vector<T>& d) {
      const auto sz{data_.size()};
      const auto n{std::min(d.size(), sz)};
      data_.assign(d.begin(), d.begin() + n);
      data_.resize(sz);
    }

  private:
    Dim rows_;
    Dim cols_;
    std::vector<T> data_;
  };

}
