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
    init_pair(int(ColorPair::Red), COLOR_RED, COLOR_BLACK);
    init_pair(int(ColorPair::Green), COLOR_GREEN, COLOR_BLACK);
    init_pair(int(ColorPair::Yellow), COLOR_YELLOW, COLOR_BLACK);
    init_pair(int(ColorPair::Blue), COLOR_BLUE, COLOR_BLACK);
    init_pair(int(ColorPair::Magenta), COLOR_MAGENTA, COLOR_BLACK);
    init_pair(int(ColorPair::Cyan), COLOR_CYAN, COLOR_BLACK);
    init_pair(int(ColorPair::White), COLOR_WHITE, COLOR_BLACK);
    init_pair(int(ColorPair::DarkGrey), 237, COLOR_BLACK);
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
