#include "X.h"

constexpr auto ITUnknown = ui::InputType::Unknown;
constexpr auto ITKeyPress = ui::InputType::KeyPress;
constexpr auto ITKeyRelease = ui::InputType::KeyRelease;
constexpr auto ITMousePress = ui::InputType::MousePress;
constexpr auto ITMouseRelease = ui::InputType::MouseRelease;
constexpr auto ITMousePosition = ui::InputType::MousePosition;

#include "Xdetail.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <stdexcept>
#include <thread>
#include <utility>

namespace {
  using namespace ui::X::detail;

  int (*oldErrorHandler)(Display*, XErrorEvent*);
  int trappedErrorCode{0};
  ui::PixelVector displaySize;
  bool grabbing{false};
  int minKeyCode, maxKeyCode, keySymsPerKeyCode;
  KeySym* xKeymap{nullptr};
  XModifierKeymap* xModmap{nullptr};
  ui::InputKeySym uiKeymap[256];
  ui::InputState uiModmap[256]{};
  ui::InputState inputState{};

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
      {XK_space, ui::InputKeySym::Space},
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

  int errorHandler(Display* display, XErrorEvent* error) {
    trappedErrorCode = error->error_code;
    return 0;
  }

  void saveKeymap() {
    XDisplayKeycodes(display, &minKeyCode, &maxKeyCode);
    xKeymap =
        XGetKeyboardMapping(display, minKeyCode, maxKeyCode - minKeyCode + 1, &keySymsPerKeyCode);
    xModmap = XGetModifierMapping(display);
  }

  void setModifierMapping(XModifierKeymap *modmap) {
    using namespace std::chrono_literals;
    while (XSetModifierMapping(display, modmap) == MappingBusy)
      std::this_thread::sleep_for(100ms);
  }

  void restoreKeymap() {
    XChangeKeyboardMapping(
        display, minKeyCode, keySymsPerKeyCode, xKeymap, maxKeyCode - minKeyCode + 1);
    XFree(xKeymap);
    xKeymap = nullptr;

    setModifierMapping(xModmap);
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
    setModifierMapping(modmap);
    XFreeModifiermap(modmap);
  }

  Cursor createCursor() {
    ::Window window = DefaultRootWindow(display);

    Pixmap source, mask;
    unsigned int width, height;
    int hotspotX, hotspotY;
    XReadBitmapFile(
        display, window, "data/cursor/cursor.xbm", &width, &height, &source, &hotspotX, &hotspotY);
    XReadBitmapFile(
        display, window, "data/cursor/cursor-mask.xbm", &width, &height, &mask, &hotspotX,
        &hotspotY);

    auto cmap{XDefaultColormap(display, DefaultScreen(display))};
    XColor fg, bg;
    fg.flags = DoRed | DoGreen | DoBlue;
    fg.red = 0;
    fg.green = 0;
    fg.blue = 0;
    bg.flags = DoRed | DoGreen | DoBlue;
    bg.red = 0;
    bg.green = 65535;
    bg.blue = 0;
    XAllocColor(display, cmap, &fg);
    XAllocColor(display, cmap, &bg);

    Cursor cursor{XCreatePixmapCursor(display, source, mask, &fg, &bg, hotspotX, hotspotY)};
    XFreePixmap(display, source);
    XFreePixmap(display, mask);
    return cursor;
  }

  void updateDisplaySize() {
    ::Window window = DefaultRootWindow(display);
    ::Window root;
    int x, y;
    unsigned int width, height, borderWidth, depth;
    XGetGeometry(display, window, &root, &x, &y, &width, &height, &borderWidth, &depth);
    ::displaySize = {ui::PixelCoordinate(width), ui::PixelCoordinate(height)};
  }

  ui::PixelRect getGeometry(Drawable d) {
    ::Window root;
    int x, y;
    unsigned int width, height, borderWidth, depth;
    trapErrors();
    XGetGeometry(display, d, &root, &x, &y, &width, &height, &borderWidth, &depth);
    using C = ui::PixelCoordinate;
    return untrapErrors() ? ui::PixelRect{} : ui::PixelRect{{C(x), C(y)}, {C(width), C(height)}};
  }
}

void ui::X::init() {
  assert(!display);
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  updateDisplaySize();
  updateActiveWindow();
}

void ui::X::finish() {
  if (display) {
    releaseInput();
    XCloseDisplay(display);
    display = nullptr;
  }
}

ui::PixelVector ui::X::displaySize() {
  return ::displaySize;
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
  saveKeymap();
  initKeymap();
  XAutoRepeatOff(display);
  XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  Cursor cursor{createCursor()};
  XGrabPointer(
      display, activeWindow, False,
      ButtonPressMask | ButtonReleaseMask | PointerMotionMask | PointerMotionHintMask,
      GrabModeAsync, GrabModeAsync, activeWindow, cursor, CurrentTime);
  XFreeCursor(display, cursor);
  grabbing = true;
}

void ui::X::releaseInput() {
  if (grabbing) {
    XUngrabPointer(display, CurrentTime);
    XUngrabKeyboard(display, CurrentTime);
    XAutoRepeatOn(display);
    restoreKeymap();
    grabbing = false;
  }
}

bool ui::X::pendingEvent() {
  return XPending(display) > 0;
}

ui::InputEvent ui::X::nextEvent() {
  XEvent event;
  XNextEvent(display, &event);

  InputEvent ievent{ITUnknown, inputState};

  switch (event.type) {
    case KeyPress:
    case KeyRelease:
      ievent.type = (event.type == KeyPress) ? ITKeyPress : ITKeyRelease;
      ievent.state |= event.xkey.state & ButtonMask;
      ievent.symbol = uiKeymap[event.xkey.keycode];
      if (InputState modState{uiModmap[event.xkey.keycode]}) {
        if (event.type == KeyPress)
          inputState |= modState;
        else
          inputState &= ~modState;
      }
      break;
    case ButtonPress:
    case ButtonRelease:
      ievent.type = (event.type == ButtonPress) ? ITMousePress : ITMouseRelease;
      ievent.state |= event.xbutton.state & ButtonMask;
      ievent.mouseButton = InputButton(event.xbutton.button);
      ievent.mousePosition.area = InputMouseArea{};
      ievent.mousePosition.point = {event.xbutton.x, event.xbutton.y};
      break;
    case MotionNotify: {
      ::Window root, child;
      int rootX, rootY, pointerX, pointerY;
      unsigned int mask;
      if (XQueryPointer(
              display, activeWindow, &root, &child, &rootX, &rootY, &pointerX, &pointerY, &mask)) {
        ievent.type = ITMousePosition;
        ievent.state |= mask & ButtonMask;
        ievent.mousePosition.area = InputMouseArea{};
        ievent.mousePosition.point = {pointerX, pointerY};
      }
      break;
    }
    default:
      break;
  }

  return ievent;
}

void ui::X::detail::trapErrors() {
  trappedErrorCode = 0;
  oldErrorHandler = XSetErrorHandler(errorHandler);
}

int ui::X::detail::untrapErrors() {
  XSetErrorHandler(oldErrorHandler);
  return trappedErrorCode;
}

void ui::X::detail::updateActiveWindow() {
  activeWindow = 0;

  Atom property{XInternAtom(display, "_NET_ACTIVE_WINDOW", True)};
  if (property != None) {
    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long bytesAfter;
    unsigned char* prop;
    if (XGetWindowProperty(
            display, DefaultRootWindow(display), property, 0, sizeof(::Window), False,
            AnyPropertyType, &actualType, &actualFormat, &nitems, &bytesAfter, &prop) == Success) {
      if (nitems > 0)
        activeWindow = *((::Window*)prop);
    }
    XFree(prop);
  }
  if (!activeWindow) {
    // fallback mechanism, not so accurate
    int revertTo;
    XGetInputFocus(display, &activeWindow, &revertTo);
  }
  updateActiveWindowRect();
}

void ui::X::detail::updateActiveWindowRect() {
  activeWindowRect = getGeometry(activeWindow);
}

Display* ui::X::detail::display{nullptr};
Window ui::X::detail::activeWindow;
ui::PixelRect ui::X::detail::activeWindowRect;
