#include "X.h"

constexpr auto ITUnknown = ui::InputType::Unknown;
constexpr auto ITKeyPress = ui::InputType::KeyPress;
constexpr auto ITKeyRelease = ui::InputType::KeyRelease;

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <utility>

namespace {
  Display* display{nullptr};
  Window terminalWindow;
  bool grabbing{false};
  int minKeyCode, maxKeyCode, keySymsPerKeyCode;
  KeySym* xKeymap{nullptr};
  XModifierKeymap* xModmap{nullptr};
  ui::InputKeySym uiKeymap[256];
  ui::InputState uiModmap[256]{};

  const std::map<KeySym, ui::InputKeySym> keysymMap{
      {XK_Tab, ui::InputKeySym::Tab},
      {XK_Escape, ui::InputKeySym::Escape},
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
      {XK_9, ui::InputKeySym::Digit_9},
      {XK_a, ui::InputKeySym::A},
      {XK_b, ui::InputKeySym::B},
      {XK_c, ui::InputKeySym::C},
      {XK_d, ui::InputKeySym::D},
      {XK_e, ui::InputKeySym::E},
      {XK_f, ui::InputKeySym::F},
      {XK_g, ui::InputKeySym::G},
      {XK_h, ui::InputKeySym::H},
      {XK_i, ui::InputKeySym::I},
      {XK_j, ui::InputKeySym::J},
      {XK_k, ui::InputKeySym::K},
      {XK_l, ui::InputKeySym::L},
      {XK_m, ui::InputKeySym::M},
      {XK_n, ui::InputKeySym::N},
      {XK_o, ui::InputKeySym::O},
      {XK_p, ui::InputKeySym::P},
      {XK_q, ui::InputKeySym::Q},
      {XK_r, ui::InputKeySym::R},
      {XK_s, ui::InputKeySym::S},
      {XK_t, ui::InputKeySym::T},
      {XK_u, ui::InputKeySym::U},
      {XK_v, ui::InputKeySym::V},
      {XK_w, ui::InputKeySym::W},
      {XK_x, ui::InputKeySym::X},
      {XK_y, ui::InputKeySym::Y},
      {XK_z, ui::InputKeySym::Z}};

  const std::map<KeySym, ui::InputState> keysymMod{
      {XK_Shift_L, ui::ShiftPressed},       {XK_Shift_R, ui::ShiftPressed},
      {XK_Control_L, ui::ControlPressed},   {XK_Control_R, ui::ControlPressed},
      {XK_Alt_L, ui::AltPressed},           {XK_Alt_R, ui::AltPressed},
      {XK_ISO_Level3_Shift, ui::AltPressed}};

  void saveKeymap() {
    XDisplayKeycodes(display, &minKeyCode, &maxKeyCode);
    xKeymap =
        XGetKeyboardMapping(display, minKeyCode, maxKeyCode - minKeyCode + 1, &keySymsPerKeyCode);
    xModmap = XGetModifierMapping(display);
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

  void initKeymap() {
    assert(xKeymap);
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

    auto* modmap{XNewModifiermap(0)};
    XSetModifierMapping(display, modmap);
    XFreeModifiermap(modmap);
  }

  void getDisplaySize() {
    ::Window window = DefaultRootWindow(display);
    ::Window root;
    int x, y;
    unsigned int borderWidth, depth;
    XGetGeometry(
        display, window, &root, &x, &y, &ui::X::displayWidth, &ui::X::displayHeight, &borderWidth,
        &depth);
  }

  void setCursor() {
    // this has no effect in terminals like xfce4-terminal, but fixes the
    // cursor in e.g. suckless st
    Cursor cursor{XCreateFontCursor(display, XC_left_ptr)};
    XDefineCursor(display, terminalWindow, cursor);
    XFreeCursor(display, cursor);
  }

  void restoreCursor() {
    // there is no easy way to save and restore the current cursor (!)
    XUndefineCursor(display, terminalWindow);
  }
}

void ui::X::init() {
  assert(!display);
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  // quite safely assume the parent terminal has the focus
  int revertTo;
  XGetInputFocus(display, &terminalWindow, &revertTo);

  setCursor();
  saveKeymap();
  initKeymap();
  XAutoRepeatOff(display);
  getDisplaySize();
}

void ui::X::finish() {
  if (display) {
    releaseInput();
    XAutoRepeatOn(display);
    restoreKeymap();
    restoreCursor();
    XCloseDisplay(display);
    display = nullptr;
  }
}

void ui::X::saveState() {
  assert(!display);
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  saveKeymap();
  XCloseDisplay(display);
  display = nullptr;
}

void ui::X::restoreState() {
  assert(!display);
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  XAutoRepeatOn(display);
  restoreKeymap();
  XCloseDisplay(display);
  display = nullptr;
}

void ui::X::grabInput() {
  assert(!grabbing);
  ::Window window = DefaultRootWindow(display);
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
  ::Window window = DefaultRootWindow(display);
  ::Window root, child;
  int winX, winY;
  unsigned int mask;
  if (XQueryPointer(display, window, &root, &child, &pointerX, &pointerY, &winX, &winY, &mask))
    inputState = (inputState & ~ButtonMask) | (mask & ButtonMask);
}

ui::X::PixelMatrix ui::X::getImage(const PixelRect& area) {
  ::Window window = DefaultRootWindow(display);
  ui::X::PixelMatrix result(MatrixVector{area.size.x, area.size.y});
  XImage* img{XGetImage(
      display, window, area.topLeft.x, area.topLeft.y, area.size.x, area.size.y, AllPlanes,
      XYPixmap)};
  for (auto p : result.rect().points())
    result[p] = XGetPixel(img, p.x, p.y);
  XFree(img);
  return result;
}

unsigned long ui::X::getPixel(PixelPoint p) {
  ::Window window = DefaultRootWindow(display);
  XImage* img{XGetImage(display, window, p.x, p.y, 1, 1, AllPlanes, XYPixmap)};
  auto result = XGetPixel(img, 0, 0);
  XFree(img);
  return result;
}

unsigned int ui::X::displayWidth, ui::X::displayHeight;
int ui::X::pointerX, ui::X::pointerY;
ui::InputState ui::X::inputState{0};
