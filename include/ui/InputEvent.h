#pragma once

#include "rts/types.h"

namespace ui {
  enum class InputType { Unknown, KeyPress, KeyRelease, MousePress, MouseRelease, EdgeScroll };

  enum class InputKeySym {
    Unknown,
    Escape,
    Left,
    Right,
    Up,
    Down,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12
  };

  using InputState = unsigned int;
  constexpr InputState ShiftPressed{0b0001};
  constexpr InputState ControlPressed{0b0100};
  constexpr InputState AltPressed{0b1000};

  using InputKeyCode = unsigned int;
  enum class InputButton { Unknown, Button1, Button2, Button3 };

  using ScrollDirection = unsigned int;
  constexpr ScrollDirection ScrollDirectionLeft{0b0001};
  constexpr ScrollDirection ScrollDirectionRight{0b0010};
  constexpr ScrollDirection ScrollDirectionUp{0b0100};
  constexpr ScrollDirection ScrollDirectionDown{0b1000};

  struct InputEvent {
    InputType type;
    InputState state;
    InputKeyCode keycode;
    InputKeySym symbol;
    InputButton mouseButton;
    rts::Point mouseCell;
    ScrollDirection scrollDirection;
  };
}
