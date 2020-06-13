#pragma once

namespace ui {
  enum class InputType { Unknown, KeyPress, KeyRelease };
  enum class InputKeySym {
    Unknown,
    Escape,
    Right,
    Left,
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
  using InputKeyCode = unsigned int;

  struct InputEvent {
    InputType type;
    InputState state;
    InputKeyCode keycode;
    InputKeySym symbol;
  };
}
