#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

namespace util {

  template<typename T, typename Dim = size_t>
  class Matrix {
  public:
    Matrix() : rows_{0}, cols_{0} {}
    explicit Matrix(Dim rows, Dim cols) : rows_{rows}, cols_{cols}, data_(rows * cols) {}

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

    friend Matrix operator+(Matrix a, const Matrix& b) {
      assert(a.rows() == b.rows() && a.cols() == b.cols());
      std::transform(
          a.data_.begin(), a.data_.end(), b.data_.begin(), a.data_.begin(), std::plus<T>());
      return a;
    }

    friend Matrix operator-(Matrix a, const Matrix& b) {
      assert(a.rows() == b.rows() && a.cols() == b.cols());
      std::transform(
          a.data_.begin(), a.data_.end(), b.data_.begin(), a.data_.begin(), std::minus<T>());
      return a;
    }

    void assign(const std::vector<T>& d) {
      const auto sz{data_.size()};
      const auto n{std::min(d.size(), sz)};
      data_.assign(d.begin(), d.begin() + n);
      data_.resize(sz);
    }

    T& data(size_t idx) {
      assert(idx < data_.size());
      return data_[idx];
    }
    const T& data(size_t idx) const {
      assert(idx < data_.size());
      return data_[idx];
    }

  private:
    Dim rows_;
    Dim cols_;
    std::vector<T> data_;
  };

}
