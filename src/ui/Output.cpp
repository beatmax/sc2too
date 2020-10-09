#include "ui/Output.h"

#include "IOState.h"
#include "Layout.h"
#include "MenuImpl.h"
#include "X.h"
#include "dim.h"
#include "graph.h"
#include "render.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "rts/constants.h"
#include "ui/ResourceUi.h"
#include "ui/SideUi.h"
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
    constexpr rts::GameTime MessageShowTime{4 * rts::GameTimeSecond};

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

    void drawResourceQuantities(const IOState& ios, const rts::World& w, const rts::Side& side) {
      const auto& win{ios.headerWin};
      int x = dim::defaultWinWidth;
      const auto& resources{side.resources()};
      for (auto id{resources.maxId()}; id >= resources.minId(); --id) {
        const auto& ui{getUi(w[id])};
        const auto& acc{side.resources()[id]};
        bool displayAlloc{ui.display == ResourceUi::Display::Allocated};
        x -= displayAlloc ? 13 : 9;
        wattrset(win.w, graph::white());
        graph::drawFrame(win, ui.icon().frame(), {{x, 0}, {1, 1}}, {0, 0});
        if (displayAlloc) {
          if (acc.allocated() > acc.totalSlots())
            wattrset(win.w, graph::red() | A_BOLD);
          mvwprintw(win.w, 0, x + 2, "%d/%d", acc.allocated(), acc.totalSlots());
        }
        else {
          mvwprintw(win.w, 0, x + 2, "%d", acc.available());
        }
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

    void drawProductionQueue(const IOState& ios, const rts::World& w, rts::UnitStableRef unit) {
      using ProduceState = rts::abilities::ProduceState;
      const rts::ProductionQueue& pq{w[unit->productionQueue]};
      const bool blocked{rts::Unit::state<ProduceState>(unit, w) == ProduceState::Blocked};

      const auto& win{ios.controlWin};
      const auto left{40};
      const auto right{left + (rts::MaxProductionQueueSize - 2) * (dim::cellWidth + 1)};
      const auto sideColor{getColor(w[pq.side()])};
      ScreenRect rect{{right, 4}, {dim::cellWidth, dim::cellHeight}};

      for (size_t i{0}; i < rts::MaxProductionQueueSize; ++i) {
        wattrset(win.w, blocked ? graph::darkGrey() : graph::darkGreen());
        graph::drawRect(win, boundingBox(rect));
        if (i < pq.size()) {
          wattrset(win.w, blocked ? graph::darkGrey() : sideColor);
          graph::drawFrame(win, getIcon(w[pq.type(i)]).frame(), rect, {0, 0});
        }

        if (i == 0) {
          rect.topLeft.x = left;
          rect.topLeft.y += dim::cellHeight + 1;
        }
        else {
          rect.topLeft.x += dim::cellWidth + 1;
        }
      }
    }

    void drawRallyPoints(
        const IOState& ios,
        const rts::World& w,
        const Camera& camera,
        const rts::Selection& selection) {
      for (auto* u : selection.items(w)) {
        if (u->productionQueue) {
          const rts::ProductionQueue& pq{w[u->productionQueue]};
          if (auto p{pq.rallyPoint()}) {
            auto color{graph::yellow()};
            if (auto* obj{w.object(*p)})
              highlight(ios.renderWin, camera, obj->area, color);
            else
              highlight(ios.renderWin, camera, *p, color);
          }
        }
      }
    }

    void drawSelection(
        const IOState& ios,
        const rts::World& w,
        const rts::Selection& selection,
        rts::UnitTypeId subgroupType) {
      const auto& win{ios.controlWin};

      int row{0}, col{0}, cnt{0};
      const auto left{40};
      ScreenRect rect{{left, 2}, {dim::cellWidth, dim::cellHeight}};

      const rts::Unit* last;
      for (auto* u : selection.items(w)) {
        ++cnt;
        last = u;

        if (col == 8) {
          col = 0;
          rect.topLeft.x = left;
          rect.topLeft.y += dim::cellHeight + 1;
          if (++row == 3)
            break;
        }
        wattrset(win.w, graph::green());
        graph::drawRect(win, boundingBox(rect));
        (u->type == subgroupType) ? wattrset(win.w, graph::lightGreen())
                                  : wattrset(win.w, graph::darkGreen());
        graph::drawFrame(win, getIcon(w[u->type]).frame(), rect, {0, 0});
        rect.topLeft.x += dim::cellWidth + 1;
        ++col;
      }
      if (row == 3)
        mvwprintw(win.w, rect.topLeft.y, rect.topLeft.x, "...");
      if (cnt == 1 && last->productionQueue)
        drawProductionQueue(ios, w, rts::StableRef{*last});
    }

    void drawAbilities(
        const IOState& ios, const rts::World& w, const Player& player, const rts::UnitType& type) {
      const auto& win{ios.controlWin};

      for (int row{0}; row < 3; ++row) {
        for (int col{0}; col < 5; ++col) {
          rts::AbilityInstanceIndex ai(row * 5 + col);
          assert(ai < rts::MaxUnitAbilities);
          if (auto a{type.abilities[ai].abilityId}) {
            ScreenRect rect{
                {79 + col * (dim::cellWidth + 2), 2 + row * (dim::cellHeight + 1)},
                {dim::cellWidth + 1, dim::cellHeight}};
            ScreenRect iconRect{
                rect.topLeft + rts::Vector{1, 0}, {dim::cellWidth, dim::cellHeight}};
            wattrset(win.w, graph::lightGreen());
            graph::drawRect(win, boundingBox(rect));
            wattrset(
                win.w,
                a == player.selectingAbilityTarget ? (graph::white() | A_BOLD) : graph::white());
            mvwaddch(win.w, rect.topLeft.y, rect.topLeft.x, ui::Layout::abilityKey(ai));
            wattrset(win.w, graph::green());
            graph::drawFrame(win, getIcon(w[a]).frame(), iconRect, {0, 0});
          }
        }
      }
    }

    void drawMessages(const IOState& ios, const rts::World& w, const rts::MessageList& messages) {
      const auto& win{ios.renderWin};

      constexpr int x{40};
      int y{win.maxY - 2};
      size_t i{messages.size()};

      while (i > 0) {
        auto& msg{messages[--i]};
        if ((w.time - msg.time) >= MessageShowTime)
          break;
        wattrset(win.w, graph::red());
        mvwaddstr(win.w, y, x, msg.text);
        --y;
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
  drawRallyPoints(ios_, w, camera, side.selection());
  if (player.selectionBox)
    drawBoundingBox(ios_.renderWin, camera, *player.selectionBox, graph::green());
  if (player.selectingAbilityTarget)
    highlight(ios_.renderWin, camera, ios_.mousePosition, graph::lightGreen());

  drawResourceQuantities(ios_, w, side);
  drawGameTime(ios_, engine, w);
  drawGameSpeed(ios_, engine);
  drawFps(ios_, engine);
  drawControlGroups(ios_, w, side);

  auto subgroupType{side.selection().subgroupType(w)};
  drawSelection(ios_, w, side.selection(), subgroupType);
  if (subgroupType)
    drawAbilities(ios_, w, player, w[subgroupType]);
  drawMessages(ios_, w, side.messages());

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
        ios_.headerWin.w, 0, 32, "(%d, %d) - (%d, %d) : (%d, %d) : %u : %u", camera.topLeft().x,
        camera.topLeft().y, camera.bottomRight().x, camera.bottomRight().y, ios_.mousePosition.x,
        ios_.mousePosition.y, ios_.mouseButtons, ios_.clicks);
  }

  for (const Window* win : allWins)
    wrefresh(win->w);
}
