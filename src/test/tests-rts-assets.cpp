#include "tests-rts-assets.h"

#include "rts/abilities.h"

#include <utility>

namespace {
  struct TestUi : rts::Ui {
    char repr;

    explicit TestUi(char r) : repr{r} {}
  };
}

//   0.........10........20........30........40
const std::string test::map{
    "                                        \n"  // 0
    "          rrr                           \n"  // 1
    "                                        \n"  // 2
    "                                        \n"  // 3
    "                                        \n"  // 4
    "                                        \n"  // 5
    "                                     bb \n"  // 6
    "                                     bb \n"  // 7
    "                                     bb \n"  // 8
    "                                        \n"  // 9
};

test::Simpleton::Simpleton(rts::Point p) : Inherited{p, rts::Vector{1, 1}} {
  addAbility(rts::abilities::move());
}

test::Building::Building(rts::Point p) : Inherited{p, rts::Vector{2, 3}} {
}

rts::Map::Cell test::CellCreator::operator()(char c, rts::Point p) const {
  switch (c) {
    case 'b':
      return Building::create(p);
    case 'r':
      return rts::Blocker{std::make_unique<TestUi>('r')};
    default:
      return rts::Map::Free{};
  }
}

char test::repr(const rts::Ui& ui) {
  return static_cast<const TestUi&>(ui).repr;
}
