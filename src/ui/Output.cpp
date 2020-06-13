#include "ui/Output.h"

#include "MenuImpl.h"
#include "X.h"
#include "dim.h"
#include "graph.h"
#include "render.h"
#include "ui/ResourceUi.h"

#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

namespace ui {
  namespace {
    WINDOW* headerWin{nullptr};
    WINDOW* renderWin{nullptr};
    WINDOW* controlWin{nullptr};
    std::vector<WINDOW*> allWins;
    bool termResized{false};
    bool termTooSmall{false};

    void handleWinch(int) { termResized = true; }

    void initWinch() {
      struct sigaction sa;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = 0;
      sa.sa_handler = handleWinch;
      sigaction(SIGWINCH, &sa, nullptr);
    }

    WINDOW* addWin(int lines, int cols, int beginY, int beginX) {
      WINDOW* win = newwin(lines, cols, beginY, beginX);
      allWins.push_back(win);
      return win;
    }

    void delAllWins() {
      for (WINDOW* win : allWins)
        delwin(win);
      allWins.clear();
    }

    void initWins() {
      int maxY, maxX;
      getmaxyx(stdscr, maxY, maxX);
      termTooSmall = maxY < dim::totalHeight || maxX < dim::totalWidth;

      const int top{maxY < dim::totalHeight ? 0 : (maxY - dim::totalHeight) / 2};
      const int left{maxX < dim::totalWidth ? 0 : (maxX - dim::totalWidth) / 2};

      headerWin = addWin(
          dim::headerWinHeight, dim::defaultWinWidth, top + dim::headerWinTop,
          left + dim::defaultWinLeft);
      renderWin = addWin(
          dim::renderWinHeight, dim::defaultWinWidth, top + dim::renderWinTop,
          left + dim::defaultWinLeft);
      controlWin = addWin(
          dim::controlWinHeight, dim::defaultWinWidth, top + dim::controlWinTop,
          left + dim::defaultWinLeft);

      graph::drawBorders({headerWin, renderWin, controlWin});
      refresh();
    }

    void onTermResized() {
      endwin();
      refresh();
      clear();
      delAllWins();
      initWins();
    }

    void drawResourceQuantities(const Player& player) {
      int x = dim::defaultWinWidth;
      const rts::ResourceMap& resources{player.side->resources()};
      for (auto it = resources.rbegin(); it != resources.rend(); ++it) {
        x -= 10;
        mvwprintw(headerWin, 0, x, "%c: %u", repr(*it->first), it->second);
      }
    }
  }
}

ui::Output::~Output() {
  endwin();
}

void ui::Output::init(Menu& menu) {
  menu_ = &menu;
  initWinch();
  graph::init();
  initWins();
}

void ui::Output::update(const rts::World& world, const Player& player) {
  if (termResized) {
    termResized = false;
    onTermResized();
  }

  werase(renderWin);
  grid(renderWin);
  render(renderWin, world, player.camera);

  werase(headerWin);
  drawResourceQuantities(player);

  if (termTooSmall) {
    werase(headerWin);
    mvwprintw(
        headerWin, 0, 0,
        "=== PLEASE RESIZE TERMINAL TO AT LEAST %d LINES AND %d COLUMNS (press Q to quit) ===",
        dim::totalHeight, dim::totalWidth);
    if (!menu_->active()) {
      menu_->show();
      X::finish();
    }
  }
  else if (menu_->active()) {
    MenuImpl::print(*menu_, headerWin);
    mvwprintw(headerWin, 0, 40, "=== GAME PAUSED ===");
  }
  else {
    const auto& c{player.camera};
    mvwprintw(headerWin, 0, 0, "F10:menu");
    mvwprintw(
        headerWin, 0, 40, "(%d, %d) - (%d, %d)", c.topLeft().x, c.topLeft().y, c.bottomRight().x,
        c.bottomRight().y);
  }

  wrefresh(headerWin);
  wrefresh(renderWin);
  wrefresh(controlWin);
}
