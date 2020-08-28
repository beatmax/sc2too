#pragma once

#include "util/Matrix.h"

#ifdef HAS_NCURSESW_NCURSES_H
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

namespace ui {
  class Frame : public util::Matrix<cchar_t, int> {
  public:
    using util::Matrix<cchar_t, int>::Matrix;
  };
}
