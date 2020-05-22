#include "tests-rts-assets.h"

#include "rts/abilities.h"

//   0.........10........20........30........40
const std::string test::map{
    "                                        \n"  // 0
    "          rrr                           \n"  // 1
    "                                        \n"  // 2
    "                                        \n"  // 3
    "                                        \n"  // 4
    "                                        \n"  // 5
    "                                        \n"  // 6
    "                                        \n"  // 7
    "                                        \n"  // 8
    "                                        \n"  // 9
};

test::Simpleton::Simpleton(rts::Position p) : Inherited{p} {
  addAbility(rts::abilities::move());
}
