#include "X.h"

constexpr auto ITUnknown = ui::InputType::Unknown;
constexpr auto ITKeyPress = ui::InputType::KeyPress;
constexpr auto ITKeyRelease = ui::InputType::KeyRelease;
constexpr auto ITEdgeScroll = ui::InputType::EdgeScroll;

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <map>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace {
  Display* display{nullptr};
  unsigned int displayWidth, displayHeight;
  std::map<KeyCode, ui::InputKeySym> keyMap;
  ui::ScrollDirection edgeScrollDirection{};

  constexpr std::pair<KeySym, ui::InputKeySym> keysymPairs[]{
      {XK_Escape, ui::InputKeySym::Escape}, {XK_Left, ui::InputKeySym::Left},
      {XK_Right, ui::InputKeySym::Right},   {XK_Up, ui::InputKeySym::Up},
      {XK_Down, ui::InputKeySym::Down},     {XK_F1, ui::InputKeySym::F1},
      {XK_F2, ui::InputKeySym::F2},         {XK_F3, ui::InputKeySym::F3},
      {XK_F4, ui::InputKeySym::F4},         {XK_F5, ui::InputKeySym::F5},
      {XK_F6, ui::InputKeySym::F6},         {XK_F7, ui::InputKeySym::F7},
      {XK_F8, ui::InputKeySym::F8},         {XK_F9, ui::InputKeySym::F9},
      {XK_F10, ui::InputKeySym::F10},       {XK_F11, ui::InputKeySym::F11},
      {XK_F12, ui::InputKeySym::F12}};

  void initKeyMap() {
    for (auto [xkKeySym, inputKeySym] : keysymPairs)
      keyMap[XKeysymToKeycode(display, xkKeySym)] = inputKeySym;
  }

  ui::InputKeySym toInputKeySym(KeyCode keycode) {
    auto it = keyMap.find(keycode);
    return it != keyMap.end() ? it->second : ui::InputKeySym::Unknown;
  }

  void getDisplaySize() {
    Window window = DefaultRootWindow(display);
    Window root;
    int x, y;
    unsigned int borderWidth, depth;
    XGetGeometry(
        display, window, &root, &x, &y, &displayWidth, &displayHeight, &borderWidth, &depth);
  }

  std::tuple<int, int, unsigned int> queryPointer() {
    Window window = DefaultRootWindow(display);
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    if (!XQueryPointer(display, window, &root, &child, &rootX, &rootY, &winX, &winY, &mask))
      return {-1, -1, 0};
    else
      return {rootX, rootY, mask};
  }
}

void ui::X::init() {
  display = XOpenDisplay(NULL);
  if (!display)
    throw std::runtime_error{"cannot connect to X server"};

  XAutoRepeatOff(display);
  getDisplaySize();
}

void ui::X::finish() {
  if (display) {
    XUngrabKeyboard(display, CurrentTime);
    XAutoRepeatOn(display);
    XCloseDisplay(display);
    display = nullptr;
  }
}

void ui::X::captureInput() {
  Window window = DefaultRootWindow(display);
  XGrabKeyboard(display, window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  initKeyMap();
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
    ievent.state = event.xkey.state;
    ievent.keycode = event.xkey.keycode;
    ievent.symbol = toInputKeySym(event.xkey.keycode);
  }

  return ievent;
}

std::optional<ui::InputEvent> ui::X::pointerEvent() {
  auto [x, y, state] = queryPointer();
  auto hDirection{(x == 0) ? ScrollDirectionLeft
                           : (x == int(displayWidth - 1)) ? ScrollDirectionRight : 0};
  auto vDirection{(y == 0) ? ScrollDirectionUp
                           : (y == int(displayHeight - 1)) ? ScrollDirectionDown : 0};
  ScrollDirection direction{hDirection | vDirection};

  if (edgeScrollDirection != direction) {
    edgeScrollDirection = direction;

    InputEvent ievent;
    ievent.state = state;
    ievent.type = ITEdgeScroll;
    ievent.scrollDirection = direction;
    return ievent;
  }
  return std::nullopt;
}

ui::InputState ui::X::inputState() {
  auto [x, y, state] = queryPointer();
  return state;
}
