#pragma once

#include "rts/types.h"

#include <optional>

namespace ui {
  enum class InputType {
    Unknown,
    KeyPress,
    KeyRelease,
    MousePress,
    MouseRelease,
    MousePosition,
    EdgeScroll
  };

  enum class InputKeySym {
    Unknown,
    Tab,
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
    F12,
    Shift_L,
    Shift_R,
    Control_L,
    Control_R,
    Alt_L,
    Alt_R,
    Digit_0,
    Digit_1,
    Digit_2,
    Digit_3,
    Digit_4,
    Digit_5,
    Digit_6,
    Digit_7,
    Digit_8,
    Digit_9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z
  };

  inline int getDigit(InputKeySym symbol) {
    return (symbol >= InputKeySym::Digit_0 && symbol <= InputKeySym::Digit_9)
        ? int(symbol) - int(InputKeySym::Digit_0)
        : -1;
  }

  using InputState = unsigned int;
  constexpr InputState ShiftPressed{0b0001};
  constexpr InputState ControlPressed{0b0100};
  constexpr InputState AltPressed{0b1000};
  constexpr InputState Button1Pressed{0b0000100000000};
  constexpr InputState Button2Pressed{0b0001000000000};
  constexpr InputState Button3Pressed{0b0010000000000};
  constexpr InputState ButtonMask{0b0011100000000};

  enum class InputButton { Unknown, Button1, Button2, Button3 };

  using ScrollDirection = unsigned int;
  constexpr ScrollDirection ScrollDirectionLeft{0b0001};
  constexpr ScrollDirection ScrollDirectionRight{0b0010};
  constexpr ScrollDirection ScrollDirectionUp{0b0100};
  constexpr ScrollDirection ScrollDirectionDown{0b1000};

  struct InputEvent {
    InputType type;
    InputState state;
    InputKeySym symbol;
    InputButton mouseButton;
    std::optional<rts::Point> mouseCell;
    ScrollDirection scrollDirection;

    operator bool() const { return type != InputType::Unknown; }
  };
}
