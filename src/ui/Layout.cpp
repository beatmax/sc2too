#include "Layout.h"

#include "rts/constants.h"

#include <cassert>

bool ui::Layout::cancel(InputKeySym symbol) {
  return symbol == InputKeySym::B || symbol == InputKeySym::Escape;
}

ui::ScrollDirection ui::Layout::scrollDirection(InputKeySym symbol) {
  switch (symbol) {
    case InputKeySym::Left:
      return ScrollDirectionLeft;
    case InputKeySym::Right:
      return ScrollDirectionRight;
    case InputKeySym::Up:
      return ScrollDirectionUp;
    case InputKeySym::Down:
      return ScrollDirectionDown;
    default:
      return 0;
  }
}

rts::AbilityInstanceIndex ui::Layout::abilityIndex(InputKeySym symbol) {
  switch (symbol) {
    case InputKeySym::Q:
      return rts::AbilityInstanceIndex{0};
    case InputKeySym::W:
      return rts::AbilityInstanceIndex{1};
    case InputKeySym::E:
      return rts::AbilityInstanceIndex{2};
    case InputKeySym::R:
      return rts::AbilityInstanceIndex{3};
    case InputKeySym::T:
      return rts::AbilityInstanceIndex{4};
    case InputKeySym::A:
      return rts::AbilityInstanceIndex{5};
    case InputKeySym::S:
      return rts::AbilityInstanceIndex{6};
    case InputKeySym::D:
      return rts::AbilityInstanceIndex{7};
    case InputKeySym::F:
      return rts::AbilityInstanceIndex{8};
    case InputKeySym::G:
      return rts::AbilityInstanceIndex{9};
    case InputKeySym::Z:
      return rts::AbilityInstanceIndex{10};
    case InputKeySym::X:
      return rts::AbilityInstanceIndex{11};
    case InputKeySym::C:
      return rts::AbilityInstanceIndex{12};
    case InputKeySym::V:
      return rts::AbilityInstanceIndex{13};
    case InputKeySym::B:
      return rts::AbilityInstanceIndex{14};
    default:
      return rts::AbilityInstanceIndex::None;
  }
}

rts::AbilityInstanceIndex ui::Layout::abilityIndex(InputKeySym symbol, rts::AbilityPage page) {
  auto idx{abilityIndex(symbol)};
  if (idx != rts::AbilityInstanceIndex::None)
    idx = rts::AbilityInstanceIndex{size_t(idx) + page * rts::MaxUnitAbilitiesPerPage};
  return idx;
}

char ui::Layout::abilityKey(rts::AbilityInstanceIndex ai) {
  static_assert(rts::MaxUnitAbilitiesPerPage == 15);
  static char key[15]{'Q', 'W', 'E', 'R', 'T', 'A', 'S', 'D', 'F', 'G', 'Z', 'X', 'C', 'V', 'B'};
  return key[ai % 15];
}
