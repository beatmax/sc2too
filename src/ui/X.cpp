#include "X.h"

constexpr auto ITUnknown = ui::InputType::Unknown;
constexpr auto ITKeyPress = ui::InputType::KeyPress;
constexpr auto ITKeyRelease = ui::InputType::KeyRelease;

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <utility>

namespace {
  Display* display{nullptr};
  bool grabbing{false};
  int minKeyCode, maxKeyCode, keySymsPerKeyCode;
  KeySym* xKeymap{nullptr};
  XModifierKeymap* xModmap{nullptr};
  ui::InputKeySym uiKeymap[256];
  ui::InputState uiModmap[256]{};

  const std::map<KeySym, ui::InputKeySym> keysymMap{{XK_Escape, ui::InputKeySym::Escape},
                                                    {XK_Left, ui::InputKeySym::Left},
                                                    {XK_Right, ui::InputKeySym::Right},
                                                    {XK_Up, ui::InputKeySym::Up},
                                                    {XK_Down, ui::InputKeySym::Down},
                                                    {XK_F1, ui::InputKeySym::F1},
                                                    {XK_F2, ui::InputKeySym::F2},
                                                    {XK_F3, ui::InputKeySym::F3},
                                                    {XK_F4, ui::InputKeySym::F4},
                                                    {XK_F5, ui::InputKeySym::F5},
                                                    {XK_F6, ui::InputKeySym::F6},
                                                    {XK_F7, ui::InputKeySym::F7},
                                                    {XK_F8, ui::InputKeySym::F8},
                                                    {XK_F9, ui::InputKeySym::F9},
                                                    {XK_F10, ui::InputKeySym::F10},
                                                    {XK_F11, ui::InputKeySym::F11},
                                                    {XK_F12, ui::InputKeySym::F12},
                                                    {XK_Shift_L, ui::InputKeySym::Shift_L},
                                                    {XK_Shift_R, ui::InputKeySym::Shift_R},
                                                    {XK_Control_L, ui::InputKeySym::Control_L},
                                                    {XK_Control_R, ui::InputKeySym::Control_R},
                                                    {XK_Alt_L, ui::InputKeySym::Alt_L},
                                                    {XK_Alt_R, ui::InputKeySym::Alt_R},
                                                    {XK_ISO_Level3_Shift, ui::InputKeySym::Alt_R},
                                                    {XK_0, ui::InputKeySym::Digit_0},
                                                    {XK_1, ui::InputKeySym::Digit_1},
                                                    {XK_2, ui::InputKeySym::Digit_2},
                                                    {XK_3, ui::InputKeySym::Digit_3},
                                                    {XK_4, ui::InputKeySym::Digit_4},
                                                    {XK_5, ui::InputKeySym::Digit_5},
                                                    {XK_6, ui::InputKeySym::Digit_6},
                                                    {XK_7, ui::InputKeySym::Digit_7},
                                                    {XK_8, ui::InputKeySym::Digit_8},
                                                    {XK_9, ui::InputKeySym::Digit_9}};

  const std::map<KeySym, ui::InputState> keysymMod{
      {XK_Shift_L, ui::ShiftPressed},       {XK_Shift_R, ui::ShiftPressed},
      {XK_Control_L, ui::ControlPressed},   {XK_Control_R, ui::ControlPressed},
      {XK_Alt_L, ui::AltPressed},           {XK_Alt_R, ui::AltPressed},
      {XK_ISO_Level3_Shift, ui::AltPressed}};

  void initKeymap() {
    XDisplayKeycodes(display, &minKeyCode, &maxKeyCode);
    xKeymap =
        XGetKeyboardMapping(display, minKeyCode, maxKeyCode - minKeyCode + 1, &keySymsPerKeyCode);

    std::fill_n(uiKeymap, 256, ui::InputKeySym::Unknown);
    for (auto keyCode = minKeyCode; keyCode <= maxKeyCode; ++keyCode) {
      KeySym xkKeySym{xKeymap[(keyCode - minKeyCode) * keySymsPerKeyCode]};

      if (auto it = keysymMap.find(xkKeySym); it != keysymMap.end())
        uiKeymap[keyCode] = it->second;

      if (auto it = keysymMod.find(xkKeySym); it != keysymMod.end()) {
        uiModmap[keyCode] = it->second;

        // unmap modifier keys to prevent window manager and terminal interference
        KeySym noSymbol{NoSymbol};
        XChangeKeyboardMapping(display, keyCode, 1, &noSymbol, 1);
      }
    }

    xModmap = XGetModifierMapping(display);
    auto* modmap{XNewModifiermap(0)};
    XSetModifierMapping(display, modmap);
    XFreeModifiermap(modmap);
  }

  void restoreKeymap() {
    XChangeKeyboardMapping(
        display, minKeyCode, keySymsPerKeyCode, xKeymap, maxKeyCode - minKeyCode + 1);
    XFree(xKeymap);
    xKeymap = nullptr;

    XSetModifierMapping(display, xModmap);
    XFreeModifiermap(xModmap);
    xModmap = nullptr;
  }

  void getDisplaySize() {
    Window window = DefaultRootWindow(display);
    Window root;
    int x, y;
    unsigned int borderWidth, depth;
    XGetGeometry(
        display, window, &root, &x, &y, &ui::X::displayWidth, &ui::X::displayHeight, &borderWidth,
        &depth);
  }
}

void ui::X::init() {
  assert(!display);
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  initKeymap();
  XAutoRepeatOff(display);
  getDisplaySize();
}

void ui::X::finish() {
  if (display) {
    releaseInput();
    XAutoRepeatOn(display);
    restoreKeymap();
    XCloseDisplay(display);
    display = nullptr;
  }
}

void ui::X::grabInput() {
  assert(!grabbing);
  Window window = DefaultRootWindow(display);
  XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  grabbing = true;
}

void ui::X::releaseInput() {
  if (grabbing) {
    XUngrabKeyboard(display, CurrentTime);
    grabbing = false;
  }
}

bool ui::X::pendingEvent() {
  return XPending(display) > 0;
}

ui::InputEvent ui::X::nextEvent() {
  XEvent event;
  XNextEvent(display, &event);

  InputEvent ievent;
  if (event.type == KeyPress)
    ievent.type = ITKeyPress;
  else if (event.type == KeyRelease)
    ievent.type = ITKeyRelease;
  else
    ievent.type = ITUnknown;

  if (event.type == KeyPress || event.type == KeyRelease) {
    ievent.state = inputState;
    ievent.symbol = uiKeymap[event.xkey.keycode];

    if (InputState modState = uiModmap[event.xkey.keycode]) {
      if (event.type == KeyPress)
        inputState |= modState;
      else
        inputState &= ~modState;
    }
  }

  return ievent;
}

void ui::X::updatePointerState() {
  Window window = DefaultRootWindow(display);
  Window root, child;
  int winX, winY;
  unsigned int mask;
  if (XQueryPointer(display, window, &root, &child, &pointerX, &pointerY, &winX, &winY, &mask))
    inputState = (inputState & ~ButtonMask) | (mask & ButtonMask);
}

unsigned int ui::X::displayWidth, ui::X::displayHeight;
int ui::X::pointerX, ui::X::pointerY;
ui::InputState ui::X::inputState{0};
