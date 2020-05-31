#include "graph.h"

namespace ui::graph {
  namespace {
    void drawBordersNoBottom(WINDOW* win, int ulChar, int urChar) {
      int begY, begX, maxY, maxX;
      getbegyx(win, begY, begX);
      getmaxyx(win, maxY, maxX);

      mvaddch(begY - 1, begX - 1, ulChar);
      hline(0, maxX);
      mvaddch(begY - 1, begX + maxX, urChar);
      mvvline(begY, begX - 1, 0, maxY);
      mvvline(begY, begX + maxX, 0, maxY);
    }

    void drawBottomBorder(WINDOW* win) {
      int begY, begX, maxY, maxX;
      getbegyx(win, begY, begX);
      getmaxyx(win, maxY, maxX);

      mvaddch(begY + maxY, begX - 1, ACS_LLCORNER);
      hline(0, maxX);
      mvaddch(begY + maxY, begX + maxX, ACS_LRCORNER);
    }

  }
}

void ui::graph::init() {
  initscr();

  raw();
  noecho();
  keypad(stdscr, true);
  curs_set(0);

  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(int(ColorPair::Red), COLOR_RED, -1);
    init_pair(int(ColorPair::Green), COLOR_GREEN, -1);
    init_pair(int(ColorPair::Yellow), COLOR_YELLOW, -1);
    init_pair(int(ColorPair::Blue), COLOR_BLUE, -1);
    init_pair(int(ColorPair::Magenta), COLOR_MAGENTA, -1);
    init_pair(int(ColorPair::Cyan), COLOR_CYAN, -1);
    init_pair(int(ColorPair::White), COLOR_WHITE, -1);
    init_pair(int(ColorPair::DarkGrey), 237, -1);
  }
}

void ui::graph::drawBorders(const std::vector<WINDOW*>& windows) {
  attrset(white() | A_BOLD);
  auto it = windows.begin();
  if (it == windows.end())
    return;
  drawBordersNoBottom(*it++, ACS_ULCORNER, ACS_URCORNER);
  while (it != windows.end())
    drawBordersNoBottom(*it++, ACS_LTEE, ACS_RTEE);
  drawBottomBorder(windows.back());
}
