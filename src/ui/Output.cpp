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
#include "ui/Sprite.h"

#include <cassert>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

namespace ui {
  namespace {
    std::vector<const Window*> allWins;
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

    void newWin(Window* win, int lines, int cols, int beginY, int beginX) {
      win->w =
          newwin(win->maxY = lines, win->maxX = cols, win->beginY = beginY, win->beginX = beginX);
      allWins.push_back(win);
    }

    void delAllWins() {
      for (const Window* win : allWins)
        delwin(win->w);
      allWins.clear();
    }

    void initWins(IOState& ios) {
      int maxY, maxX;
      getmaxyx(stdscr, maxY, maxX);
      termTooSmall = maxY < dim::totalHeight || maxX < dim::totalWidth;

      const int top{maxY < dim::totalHeight ? 0 : (maxY - dim::totalHeight) / 2};
      const int left{maxX < dim::totalWidth ? 0 : (maxX - dim::totalWidth) / 2};

      newWin(
          &ios.headerWin, dim::headerWinHeight, dim::defaultWinWidth, top + dim::headerWinTop,
          left + dim::defaultWinLeft);
      newWin(
          &ios.renderWin, dim::renderWinHeight, dim::defaultWinWidth, top + dim::renderWinTop,
          left + dim::defaultWinLeft);
      newWin(
          &ios.controlWin, dim::controlWinHeight, dim::defaultWinWidth, top + dim::controlWinTop,
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
        mvwprintw(ios.headerWin.w, 0, x, "%c: %u", repr(*it->first), it->second);
      }
    }

    void drawGameTime(const IOState& ios, const rts::Engine& engine, const rts::World& w) {
      const auto& win{ios.controlWin};
      auto tsec = w.time / engine.initialGameSpeed();
      wattrset(win.w, graph::green());
      mvwprintw(win.w, 0, 0, "%02d:%02d:%02d", tsec / 3600, (tsec / 60) % 60, tsec % 60);
    }

    void drawGameSpeed(const IOState& ios, const rts::Engine& engine) {
      const auto& win{ios.controlWin};
      wattrset(win.w, graph::magenta());
      mvwprintw(ios.controlWin.w, 8, 0, "Speed: %d (F11/F12)", engine.gameSpeed());
    }

    void drawFps(const IOState& ios, const rts::Engine& engine) {
      const auto& win{ios.controlWin};
      wattrset(win.w, graph::magenta());
      mvwprintw(win.w, 7, 0, "%u FPS", engine.fps());
    }

    void drawControlGroups(const IOState& ios, const rts::World& w, const rts::Side& side) {
      const auto& win{ios.controlWin};
      for (rts::ControlGroupId g{0}; g < rts::MaxControlGroups; ++g) {
        if (auto n{side.group(g).ids(w).size()}) {
          auto col{g ? g : 10};
          wattrset(win.w, graph::blue());
          mvwaddch(win.w, 0, 20 + 6 * col, '[');
          wattrset(win.w, graph::white());
          wprintw(win.w, "%d", g);
          wattrset(win.w, graph::green());
          wprintw(win.w, "%3d", n);
          wattrset(win.w, graph::blue());
          waddch(win.w, ']');
        }
      }
    }

    void drawSelection(const IOState& ios, const rts::World& w, const rts::Selection& selection) {
      const auto& win{ios.controlWin};
      wattrset(win.w, graph::green());

      int row{0}, col{0};
      const auto left{40};
      ScreenRect rect{{left, 2}, {dim::cellWidth, dim::cellHeight}};

      for (auto* e : selection.items(w)) {
        if (col == 8) {
          col = 0;
          rect.topLeft.x = left;
          rect.topLeft.y += dim::cellHeight + 1;
          if (++row == 3)
            break;
        }
        graph::drawRect(win, boundingBox(rect));
        graph::drawSprite(win, getIcon(w[e->type]), rect, {0, 0});
        rect.topLeft.x += dim::cellWidth + 1;
        ++col;
      }
      if (row == 3)
        mvwprintw(win.w, rect.topLeft.y, rect.topLeft.x, "...");
    }

    void drawAbilities(const IOState& ios, const rts::World& w, const rts::Selection& selection) {
      const auto& win{ios.controlWin};
      wattrset(win.w, graph::lightGreen());

      const auto& entities{selection.items(w)};
      if (entities.empty())
        return;
      const auto& abilities{w[entities.front()->type].abilities};

      for (int row{0}; row < 3; ++row) {
        for (int col{0}; col < 5; ++col) {
          rts::EntityAbilityIndex ea(row * 3 + col);
          assert(ea < rts::MaxEntityAbilities);
          if (auto a{abilities[ea].abilityId}) {
            ScreenRect rect{{84 + col * (dim::cellWidth + 1), 2 + row * (dim::cellHeight + 1)},
                            {dim::cellWidth, dim::cellHeight}};
            graph::drawRect(win, boundingBox(rect));
            graph::drawSprite(win, getIcon(w[a]), rect, {0, 0});
          }
        }
      }
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

  for (const Window* win : allWins)
    werase(win->w);

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
  drawControlGroups(ios_, w, side);
  drawSelection(ios_, w, side.selection());
  drawAbilities(ios_, w, side.selection());

  if (termTooSmall) {
    werase(ios_.headerWin.w);
    mvwprintw(
        ios_.headerWin.w, 0, 0,
        "=== PLEASE RESIZE TERMINAL TO AT LEAST %d LINES AND %d COLUMNS (press Q to quit) ===",
        dim::totalHeight, dim::totalWidth);
    if (!ios_.menu.active()) {
      ios_.menu.show();
      X::finish();
    }
  }
  else if (ios_.menu.active()) {
    MenuImpl::print(ios_.menu, ios_.headerWin);
    mvwprintw(ios_.headerWin.w, 0, 40, "=== GAME PAUSED ===");
  }
  else {
    mvwprintw(ios_.headerWin.w, 0, 0, "F10:menu");
    mvwprintw(
        ios_.headerWin.w, 0, 32, "(%d, %d) - (%d, %d) : (%d, %d) : %d", camera.topLeft().x,
        camera.topLeft().y, camera.bottomRight().x, camera.bottomRight().y, ios_.mousePosition.x,
        ios_.mousePosition.y, ios_.mouseButtons);
  }

  for (const Window* win : allWins)
    wrefresh(win->w);
}
