#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <utility>

namespace util {

  // Fixed size object pool, free list allocation, weak ids.

  namespace detail::pool {
    using Index = uint32_t;
    using Generation = uint32_t;

    template<typename T>
    struct Item {
      constexpr static uint32_t FreeMask{0x80000000};

      union {
        T value;
        Index nextFree;
      };
      Generation gen{FreeMask};

      Item() {}
      ~Item() {
        if (!isFree())
          value.~T();
      }

      bool isFree() const { return gen & FreeMask; }
      void setFree() { gen |= FreeMask; }
      void setUsed() { gen &= ~FreeMask; }
    };

    template<typename T, uint32_t N, typename AIt>
    class Iterator {
    public:
      Iterator(AIt it, AIt end) : it_{it}, end_{end} { advance(); }
      Iterator(AIt end) : it_{end}, end_{end} {}

      auto& operator*() { return it_->value; }
      auto* operator->() { return &it_->value; }
      Iterator& operator++() {
        advance();
        return *this;
      }
      bool operator==(Iterator other) const { return it_ == other.it_; }
      bool operator!=(Iterator other) const { return it_ != other.it_; }

    private:
      void advance() {
        while (++it_ != end_ && it_->isFree())
          ;
      }

      AIt it_;
      const AIt end_;
    };
  }

  template<typename T>
  struct PoolObjectId {
    detail::pool::Index idx{};

    explicit operator bool() const { return idx != 0; }
  };

  template<typename T>
  struct PoolObjectWeakId {
    detail::pool::Index idx{};
    detail::pool::Generation gen{};

    PoolObjectWeakId() = default;

    template<typename FL>
    PoolObjectWeakId(const FL& list, PoolObjectId<T> id)
      : idx{id.idx}, gen{list.items_[id.idx].gen} {}

    explicit operator bool() const { return idx != 0; }
    explicit operator PoolObjectId<T>() const { return {idx}; }
  };

  template<typename T, uint32_t N>
  class Pool {
    using Index = detail::pool::Index;
    using Item = detail::pool::Item<T>;
    using Array = std::array<Item, N + 1>;

  public:
    using Value = T;
    using Id = PoolObjectId<Value>;
    using WeakId = PoolObjectWeakId<Value>;
    using iterator = detail::pool::Iterator<Value, N, typename Array::iterator>;
    using const_iterator = detail::pool::Iterator<Value, N, typename Array::const_iterator>;

    Pool();

    template<typename... Args>
    Id emplace(Args&&... args);

    void erase(Id id);

    Value& operator[](Id id) {
      Item& item{items_[id.idx]};
      assert(!item.isFree());
      return item.value;
    }
    const Value& operator[](Id id) const { return const_cast<Pool&>(*this)[id]; }

    Value* operator[](WeakId wid) {
      Item& item{items_[wid.idx]};
      return item.gen == wid.gen ? &item.value : nullptr;
    }
    const Value* operator[](WeakId wid) const { return const_cast<Pool&>(*this)[wid]; }

    iterator begin() { return iterator{items_.begin(), items_.end()}; }
    iterator end() { return iterator{items_.end()}; }

    const_iterator begin() const { return const_iterator{items_.begin(), items_.end()}; }
    const_iterator end() const { return const_iterator{items_.end()}; }

    Id id(const Value& value) const {
      auto item{reinterpret_cast<const Item*>(&value)};
      assert(item > items_.data());
      assert(item < items_.data() + items_.size());
      return Id{Index(item - items_.data())};
    }

    WeakId weakId(const Value& value) const { return WeakId{*this, id(value)}; }

    bool full() const { return items_[0].nextFree == 0; }

  private:
    friend struct PoolObjectWeakId<Value>;

    Array items_;
  };

  template<typename T, uint32_t N>
  Pool<T, N>::Pool() {
    for (Index idx{0}; idx < N; ++idx)
      items_[idx].nextFree = idx + 1;
    items_[N].nextFree = 0;
  }

  template<typename T, uint32_t N>
  template<typename... Args>
  auto Pool<T, N>::emplace(Args&&... args) -> Id {
    Index& firstFree{items_[0].nextFree};
    assert(firstFree != 0);

    Index idx{firstFree};
    Item& item{items_[idx]};
    assert(item.isFree());
    Index nextFree{item.nextFree};

    assert(reinterpret_cast<char*>(&item.value) == reinterpret_cast<char*>(&item));

    new (&item.value) Value(std::forward<Args>(args)...);

    firstFree = nextFree;
    item.setUsed();

    return Id{idx};
  }

  template<typename T, uint32_t N>
  void Pool<T, N>::erase(Id id) {
    Item& item{items_[id.idx]};
    assert(!item.isFree());

    item.value.~Value();

    ++item.gen;
    item.setFree();

    Index& firstFree{items_[0].nextFree};
    item.nextFree = firstFree;
    firstFree = id.idx;
  }

}