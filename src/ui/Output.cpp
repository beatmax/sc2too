#include "ui/Output.h"

#include "dim.h"
#include "graph.h"
#include "render.h"

#include <ncurses.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace ui {
  namespace {
    WINDOW* headerWin = nullptr;
    WINDOW* renderWin = nullptr;
    WINDOW* controlWin = nullptr;
  }
}

ui::Output::Output() {
  graph::init();

  int maxY, maxX;
  getmaxyx(stdscr, maxY, maxX);
  if (maxY < dim::totalHeight || maxX < dim::totalWidth) {
    endwin();
    std::ostringstream oss;
    oss << "terminal size " << maxX << 'x' << maxY << ", should be at least " << dim::totalWidth
        << 'x' << dim::totalHeight;
    throw std::runtime_error{oss.str()};
  }

  headerWin =
      newwin(dim::headerWinHeight, dim::defaultWinWidth, dim::headerWinTop, dim::defaultWinLeft);
  renderWin =
      newwin(dim::renderWinHeight, dim::defaultWinWidth, dim::renderWinTop, dim::defaultWinLeft);
  controlWin =
      newwin(dim::controlWinHeight, dim::defaultWinWidth, dim::controlWinTop, dim::defaultWinLeft);

  graph::drawBorders({headerWin, renderWin, controlWin});
  refresh();

  grid(renderWin);
}

ui::Output::~Output() {
  endwin();
}

void ui::Output::update(const rts::World& world, const Player& player) {
  render(renderWin, world, player.camera);

  const auto& c{player.camera};
  werase(headerWin);
  mvwprintw(
      headerWin, 0, 0, "(%d, %d) - (%d, %d)", c.topLeft().x, c.topLeft().y, c.bottomRight().x,
      c.bottomRight().y);

  wrefresh(headerWin);
  wrefresh(renderWin);
  wrefresh(controlWin);
}
