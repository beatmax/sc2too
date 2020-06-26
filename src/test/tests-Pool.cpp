#include "catch2/catch.hpp"
#include "util/Pool.h"

namespace {
  int dummyCount{0};
  int dummySerial{1000};

  struct Dummy {
    Dummy() { ++dummyCount; }
    Dummy(int x) : x{x} { ++dummyCount; }
    ~Dummy() { --dummyCount; }
    int s{++dummySerial}, x{};
  };

  constexpr uint32_t MaxDummies{4};
  using DummyPool = util::Pool<Dummy, MaxDummies>;
  using DummyId = DummyPool::Id;
  using DummyWeakId = DummyPool::WeakId;
}

TEST_CASE("Objects are emplaced and erased, lifetime is tracked by weak ids", "[Pool]") {
  {
    DummyPool pool;
    const DummyPool& cpool{pool};

    auto serials = [&]() {
      std::vector<int> v;
      for (const Dummy& d : cpool)
        v.push_back(d.s);
      return v;
    };

    DummyId id1{pool.emplace(123)};
    REQUIRE(id1.idx == 1);
    REQUIRE(dummyCount == 1);

    DummyWeakId wid1{pool, id1};
    REQUIRE(wid1.idx == 1);
    REQUIRE(wid1.gen == 0);
    REQUIRE(cpool[wid1] != nullptr);
    REQUIRE(cpool[wid1]->s == 1001);

    {
      Dummy& dummy{pool[id1]};
      REQUIRE(dummy.s == 1001);
      REQUIRE(dummy.x == 123);
      REQUIRE(pool.id(dummy).idx == 1);
      wid1 = pool.weakId(dummy);
      REQUIRE(wid1.idx == 1);
      REQUIRE(wid1.gen == 0);
    }

    DummyId id2{pool.emplace()};
    REQUIRE(id2.idx == 2);
    REQUIRE(dummyCount == 2);
    {
      const Dummy& dummy{cpool[id2]};
      REQUIRE(dummy.s == 1002);
      REQUIRE(cpool.id(dummy).idx == 2);
    }

    pool.erase(id1);
    REQUIRE(dummyCount == 1);
    REQUIRE(cpool[wid1] == nullptr);

    DummyId id3{pool.emplace()};
    REQUIRE(id3.idx == 1);
    REQUIRE(dummyCount == 2);
    REQUIRE(cpool[wid1] == nullptr);

    DummyWeakId wid3{cpool, id3};
    REQUIRE(wid3.idx == 1);
    REQUIRE(wid3.gen == 1);
    REQUIRE(pool[wid3] != nullptr);
    REQUIRE(pool[wid3]->s == 1003);

    {
      Dummy& dummy{pool[id3]};
      REQUIRE(dummy.s == 1003);
      REQUIRE(pool.id(dummy).idx == 1);
      wid3 = cpool.weakId(dummy);
      REQUIRE(pool[wid3] != nullptr);
      REQUIRE(pool[wid3]->s == 1003);
    }

    DummyId id4{pool.emplace()};
    REQUIRE(id4.idx == 3);
    REQUIRE(dummyCount == 3);
    REQUIRE(!cpool.full());
    {
      Dummy& dummy{pool[id4]};
      REQUIRE(dummy.s == 1004);
      REQUIRE(cpool.id(dummy).idx == 3);
    }

    DummyId id5{pool.emplace()};
    REQUIRE(id5.idx == 4);
    REQUIRE(dummyCount == 4);
    REQUIRE(cpool.full());
    {
      Dummy& dummy{pool[id5]};
      REQUIRE(dummy.s == 1005);
      REQUIRE(cpool.id(dummy).idx == 4);
    }

    {
      auto it{pool.begin()};
      REQUIRE(it != pool.end());
      REQUIRE(it->s == 1003);
      it->x++;
      ++it;
      REQUIRE(it != pool.end());
      REQUIRE(it->s == 1002);
      ++it;
      REQUIRE(it != pool.end());
      REQUIRE(it->s == 1004);
      ++it;
      REQUIRE(it != pool.end());
      REQUIRE(it->s == 1005);
      ++it;
      REQUIRE(it == pool.end());
    }

    REQUIRE(serials() == std::vector<int>{1003, 1002, 1004, 1005});

    DummyWeakId wid2{pool, id2};
    REQUIRE(pool[wid2] != nullptr);
    REQUIRE(pool[wid2]->s == 1002);

    pool.erase(id2);
    REQUIRE(pool[wid2] == nullptr);
    REQUIRE(!pool.full());

    pool.erase(id4);
    REQUIRE(serials() == std::vector<int>{1003, 1005});
  }

  REQUIRE(dummyCount == 0);
}
