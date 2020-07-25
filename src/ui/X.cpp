#include "X.h"

constexpr auto ITUnknown = ui::InputType::Unknown;
constexpr auto ITKeyPress = ui::InputType::KeyPress;
constexpr auto ITKeyRelease = ui::InputType::KeyRelease;
constexpr auto ITEdgeScroll = ui::InputType::EdgeScroll;

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>

namespace {
  Display* display{nullptr};
  unsigned int displayWidth, displayHeight;
  int minKeyCode, maxKeyCode, keySymsPerKeyCode;
  KeySym* xKeymap{nullptr};
  XModifierKeymap* xModmap{nullptr};
  ui::InputKeySym uiKeymap[256];
  ui::InputState uiModmap[256]{};

  ui::ScrollDirection edgeScrollDirection{};

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
                                                    {XK_ISO_Level3_Shift, ui::InputKeySym::Alt_R}};

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
        display, window, &root, &x, &y, &displayWidth, &displayHeight, &borderWidth, &depth);
  }

  std::pair<int, int> queryPointer() {
    Window window = DefaultRootWindow(display);
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    if (!XQueryPointer(display, window, &root, &child, &rootX, &rootY, &winX, &winY, &mask))
      return {-1, -1};
    else
      return {rootX, rootY};
  }
}

void ui::X::init() {
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  initKeymap();
  XAutoRepeatOff(display);
  getDisplaySize();
}

void ui::X::finish() {
  if (display) {
    XUngrabKeyboard(display, CurrentTime);
    XAutoRepeatOn(display);
    restoreKeymap();
    XCloseDisplay(display);
    display = nullptr;
  }
}

void ui::X::captureInput() {
  Window window = DefaultRootWindow(display);
  XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
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

std::optional<ui::InputEvent> ui::X::pointerEvent() {
  auto [x, y] = queryPointer();
  auto hDirection{(x == 0) ? ScrollDirectionLeft
                           : (x == int(displayWidth - 1)) ? ScrollDirectionRight : 0};
  auto vDirection{(y == 0) ? ScrollDirectionUp
                           : (y == int(displayHeight - 1)) ? ScrollDirectionDown : 0};
  ScrollDirection direction{hDirection | vDirection};

  if (edgeScrollDirection != direction) {
    edgeScrollDirection = direction;

    InputEvent ievent;
    ievent.state = inputState;
    ievent.type = ITEdgeScroll;
    ievent.scrollDirection = direction;
    return ievent;
  }
  return std::nullopt;
}

ui::InputState ui::X::inputState{0};
