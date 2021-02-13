#include <X11/Xlib.h>

namespace ui::X::detail {
  void trapErrors();
  int untrapErrors();
  void updateActiveWindow();
  void updateActiveWindowRect();

  extern ::Display* display;
  extern ::Window activeWindow;
  extern PixelRect activeWindowRect;
}
