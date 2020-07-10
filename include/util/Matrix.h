#pragma once

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

    T& operator()(Dim i, Dim j) { return data_[i * cols_ + j]; }
    const T& operator()(Dim i, Dim j) const { return data_[i * cols_ + j]; }

  private:
    Dim rows_;
    Dim cols_;
    std::vector<T> data_;
  };

}
