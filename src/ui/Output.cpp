#include "ui/Output.h"

#include "IOState.h"
#include "MenuImpl.h"
#include "X.h"
#include "dim.h"
#include "graph.h"
#include "render.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "ui/ResourceUi.h"

#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

namespace ui {
  namespace {
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

    void initWins(IOState& ios) {
      int maxY, maxX;
      getmaxyx(stdscr, maxY, maxX);
      termTooSmall = maxY < dim::totalHeight || maxX < dim::totalWidth;

      const int top{maxY < dim::totalHeight ? 0 : (maxY - dim::totalHeight) / 2};
      const int left{maxX < dim::totalWidth ? 0 : (maxX - dim::totalWidth) / 2};

      ios.headerWin = addWin(
          dim::headerWinHeight, dim::defaultWinWidth, top + dim::headerWinTop,
          left + dim::defaultWinLeft);
      ios.renderWin = addWin(
          dim::renderWinHeight, dim::defaultWinWidth, top + dim::renderWinTop,
          left + dim::defaultWinLeft);
      ios.controlWin = addWin(
          dim::controlWinHeight, dim::defaultWinWidth, top + dim::controlWinTop,
          left + dim::defaultWinLeft);

      graph::drawBorders(allWins);
      refresh();
    }

    void onTermResized(IOState& ios) {
      endwin();
      refresh();
      clear();
      delAllWins();
      initWins(ios);
    }

    void drawResourceQuantities(const IOState& ios, const rts::World& w, const Player& player) {
      int x = dim::defaultWinWidth;
      const rts::ResourceMap& resources{w[player.side].resources()};
      for (auto it = resources.rbegin(); it != resources.rend(); ++it) {
        x -= 10;
        mvwprintw(ios.headerWin, 0, x, "%c: %u", repr(*it->first), it->second);
      }
    }

    void drawGameTime(const IOState& ios, const rts::Engine& engine, const rts::World& w) {
      auto tsec = w.time / engine.initialGameSpeed();
      mvwprintw(ios.controlWin, 0, 0, "%02d:%02d:%02d", tsec / 3600, (tsec / 60) % 60, tsec % 60);
    }

    void drawGameSpeed(const IOState& ios, const rts::Engine& engine) {
      mvwprintw(ios.controlWin, 1, 0, "Speed: %d (F11/F12)", engine.gameSpeed());
    }

    void drawFps(const IOState& ios, const rts::Engine& engine) {
      mvwprintw(ios.controlWin, 8, 0, "%u FPS", engine.fps());
    }
  }
}

ui::Output::Output(IOState& ios) : ios_{ios} {
}

ui::Output::~Output() {
  endwin();
}

void ui::Output::init() {
  initWinch();
  graph::init();
  initWins(ios_);
}

void ui::Output::update(const rts::Engine& engine, const rts::World& w, const Player& player) {
  const auto& camera{player.camera};
  const auto& side{w[player.side]};

  if (termResized) {
    termResized = false;
    onTermResized(ios_);
  }

  werase(ios_.renderWin);
  werase(ios_.headerWin);
  werase(ios_.controlWin);

  grid(ios_.renderWin);

#ifdef MAP_DEBUG
  mapDebug(ios_.renderWin, w, camera);
#endif

  highlight(
      ios_.renderWin, camera, ios_.clickedTarget,
      ios_.mouseButtons ? graph::red() : graph::yellow());
  render(ios_.renderWin, w, camera, side.selection());
  if (player.selectionBox)
    drawBoundingBox(ios_.renderWin, camera, *player.selectionBox, graph::green());

  drawResourceQuantities(ios_, w, player);
  drawGameTime(ios_, engine, w);
  drawGameSpeed(ios_, engine);
  drawFps(ios_, engine);

  if (termTooSmall) {
    werase(ios_.headerWin);
    mvwprintw(
        ios_.headerWin, 0, 0,
        "=== PLEASE RESIZE TERMINAL TO AT LEAST %d LINES AND %d COLUMNS (press Q to quit) ===",
        dim::totalHeight, dim::totalWidth);
    if (!ios_.menu.active()) {
      ios_.menu.show();
      X::finish();
    }
  }
  else if (ios_.menu.active()) {
    MenuImpl::print(ios_.menu, ios_.headerWin);
    mvwprintw(ios_.headerWin, 0, 40, "=== GAME PAUSED ===");
  }
  else {
    mvwprintw(ios_.headerWin, 0, 0, "F10:menu");
    mvwprintw(
        ios_.headerWin, 0, 32, "(%d, %d) - (%d, %d) : (%d, %d) : %d", camera.topLeft().x,
        camera.topLeft().y, camera.bottomRight().x, camera.bottomRight().y, ios_.mousePosition.x,
        ios_.mousePosition.y, ios_.mouseButtons);
  }

  for (auto* win : allWins)
    wrefresh(win);
}
