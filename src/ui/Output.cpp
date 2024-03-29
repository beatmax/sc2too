#include "ui/Output.h"

#include "IOState.h"
#include "Layout.h"
#include "MenuImpl.h"
#include "Minimap.h"
#include "X.h"
#include "graph.h"
#include "render.h"
#include "rts/Engine.h"
#include "rts/World.h"
#include "rts/constants.h"
#include "ui/ResourceUi.h"
#include "ui/SideUi.h"
#include "ui/Sprite.h"
#include "ui/dim.h"

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
    const wchar_t productionDots[] = L" ⠁⠉⠋⠛⠟⠿";

    std::vector<const Window*> allWins;
    bool termResized{false};
    bool termTooSmall{false};
    Minimap uiMinimap;

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
      const auto sz{graph::screenSize()};
      termTooSmall = sz.y < dim::TotalSize.y || sz.x < dim::TotalSize.x;

      const int top{sz.y < dim::TotalSize.y ? 0 : (sz.y - dim::TotalSize.y) / 2};
      const int left{sz.x < dim::TotalSize.x ? 0 : (sz.x - dim::TotalSize.x) / 2};

      newWin(
          &ios.headerWin, dim::HeaderWinHeight, dim::DefaultWinWidth, top + dim::HeaderWinTop,
          left + dim::DefaultWinLeft);
      newWin(
          &ios.renderWin, dim::RenderWinHeight, dim::DefaultWinWidth, top + dim::RenderWinTop,
          left + dim::DefaultWinLeft);
      newWin(
          &ios.controlWin, dim::ControlWinHeight, dim::DefaultWinWidth, top + dim::ControlWinTop,
          left + dim::DefaultWinLeft);

      graph::drawBorders(allWins);
      refresh();
    }

    void redraw(IOState& ios) {
      endwin();
      refresh();
      clear();
      delAllWins();
      initWins(ios);
    }

    void drawResourceQuantities(const IOState& ios, const rts::World& w, const rts::Side& side) {
      const auto& win{ios.headerWin};
      int x = dim::DefaultWinWidth;
      const auto& resources{side.resources()};
      for (auto id{resources.maxId()}; id >= resources.minId(); --id) {
        const auto& ui{getUi(w[id])};
        const auto& acc{side.resources()[id]};
        bool displayAlloc{ui.display == ResourceUi::Display::Allocated};
        x -= displayAlloc ? 13 : 9;
        graph::drawFrame(win, ui.icon().frame(), {x, 0});
        if (displayAlloc) {
          if (acc.allocated() > acc.totalSlots())
            wattr_set(win.w, A_BOLD, graph::colorPair(Color::Red), nullptr);
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
      graph::setColor(win, Color::Green);
      mvwprintw(win.w, 6, 19, "%02d:%02d:%02d", tsec / 3600, (tsec / 60) % 60, tsec % 60);
    }

    void drawGameSpeed(const IOState& ios, const rts::Engine& engine) {
      const auto& win{ios.controlWin};
      graph::setColor(win, Color::Magenta);
      mvwprintw(ios.controlWin.w, 8, 19, "Speed: %d (F11/F12)", engine.gameSpeed());
    }

    void drawFps(const IOState& ios, const rts::Engine& engine) {
      const auto& win{ios.controlWin};
      graph::setColor(win, Color::Magenta);
      mvwprintw(win.w, 7, 19, "%u FPS", engine.fps());
    }

    void drawControlGroups(const IOState& ios, const rts::World& w, const rts::Side& side) {
      const auto& win{ios.controlWin};
      for (rts::ControlGroupId g{0}; g < rts::MaxControlGroups; ++g) {
        if (auto n{side.group(g).ids(w).size()}) {
          auto col{g ? g : 10};
          graph::setColor(win, Color::Blue);
          mvwaddch(win.w, 0, 20 + 6 * col, '[');
          graph::setColor(win, Color::White);
          wprintw(win.w, "%d", g);
          graph::setColor(win, Color::Green);
          wprintw(win.w, "%3lu", n);
          graph::setColor(win, Color::Blue);
          waddch(win.w, ']');
        }
      }
    }

    void drawProductionQueue(const IOState& ios, const rts::World& w, rts::UnitStableRef unit) {
      using ProduceState = rts::abilities::ProduceState;
      const rts::ProductionQueue& pq{w[unit->productionQueue]};
      const bool frozen{
          !unit->powered ||
          rts::Unit::abilityState<ProduceState>(unit, w) == ProduceState::Blocked};

      const auto& win{ios.controlWin};
      const auto left{40};
      const auto right{left + (rts::MaxProductionQueueSize - 2) * dim::CellSizeEx.x};
      const auto iconColor{frozen ? Color::DarkGray : getColor(w[pq.side()])};
      ScreenRect rect{{right, 4}, dim::CellSize};

      for (size_t i{0}; i < rts::MaxProductionQueueSize; ++i) {
        graph::setColor(win, frozen ? Color::DarkGray : Color::DarkGreen);
        graph::drawRect(win, boundingBox(rect));
        if (i < pq.size())
          graph::drawFrame(win, getIcon(w[pq.product(i)]).frame(), rect.topLeft, iconColor);

        if (i == 0) {
          rect.topLeft.x = left;
          rect.topLeft.y += dim::CellSizeEx.y;
        }
        else {
          rect.topLeft.x += dim::CellSizeEx.x;
        }
      }
    }

    void drawPower(
        IOState& ios,
        const Camera& camera,
        const rts::Rectangle& unitArea,
        rts::Coordinate powerRadius,
        short colorPair) {
      for (auto p : circleCenteredAt(unitArea, powerRadius).points(ios.geoCache)) {
        if (camera.area().contains(p))
          drawBoundingBox(ios.renderWin, camera, p, colorPair);
      }
    }

    void drawPower(
        IOState& ios,
        const rts::World& w,
        const Camera& camera,
        const rts::Selection::Subgroup& subgroup,
        rts::Coordinate powerRadius) {
      const auto colorPair{graph::colorPair(Color::Gray)};
      for (auto id : subgroup.allIds) {
        auto& u{w[id]};
        if (u.type == subgroup.type)
          drawPower(ios, camera, u.area, powerRadius, colorPair);
      }
    }

    void drawPower(IOState& ios, const Camera& camera, const rts::PowerMap& powerMap) {
      const auto colorPair{graph::colorPair(Color::ElectricBlue2)};
      for (auto p : camera.area().points()) {
        if (powerMap.isActive(p))
          drawBoundingBox(ios.renderWin, camera, p, colorPair);
      }
    }

    void drawResourceProximity(IOState& ios, const Camera& camera, const rts::ProximityMap& pm) {
      graph::setColor(ios.renderWin, Color::DarkRed);
      for (auto p : camera.area().points()) {
        if (pm.isActive(p)) {
          auto sp{toScreenPoint(camera, p)};
          mvwaddwstr(ios.renderWin.w, sp.y, sp.x, L"███");
        }
      }
    }

    void drawTargetSelectPoint(
        const IOState& ios,
        const rts::World& w,
        const Player& player,
        const rts::Selection::Subgroup& subgroup,
        rts::AbilityInstanceIndex abilityIndex) {
      const auto& ai{*subgroup.abilities[abilityIndex]};
      const auto colorPair{graph::colorPair(
          w.compatibleTarget(ai.targetType, player.side, ios.mouseMapPoint) ? Color::BrightGreen
                                                                            : Color::Red)};
      if (auto* obj{w.object(ios.mouseMapPoint)})
        highlight(ios.renderWin, player.camera, obj->area, colorPair);
      else
        highlight(ios.renderWin, player.camera, ios.mouseMapPoint, colorPair);
    }

    void drawSelection(
        const IOState& ios,
        const rts::World& w,
        const WorldView& wv,
        rts::UnitTypeId subgroupType) {
      const auto& win{ios.controlWin};
      const auto& selection{wv.selection};

      for (auto p : MatrixRect{{0, 0}, {selection.cols(), selection.rows()}}.points()) {
        if (auto id{selection[p]}) {
          const auto& u{w[id]};
          const ScreenRect rect{
              transform(p, dim::CellSizeEx, {0, 0}, dim::SelectionArea.topLeft), dim::CellSize};

          graph::setColor(win, Color::Green);
          graph::drawRect(win, boundingBox(rect));
          const auto color = (u.type == subgroupType) ? Color::BrightGreen : Color::DarkGreen;
          graph::drawFrame(win, getIcon(w[u.type]).frame(), rect.topLeft, color);
          if (wv.selectionTotalSize > 1 && u.productionQueue) {
            if (auto sz{w[u.productionQueue].size()}) {
              assert(sz < sizeof(productionDots) / sizeof(productionDots[0]) - 1);
              mvwaddnwstr(win.w, rect.topLeft.y, rect.topLeft.x + 2, &productionDots[sz], 1);
            }
          }
        }
      }

      if (wv.selectionTotalSize > selection.size()) {
        auto pos{transform(
            MatrixPoint{0, selection.rows()}, dim::CellSizeEx, {0, 0}, dim::SelectionArea.topLeft)};
        mvwprintw(win.w, pos.y, pos.x, "...");
      }
      if (wv.selectionTotalSize == 1) {
        assert(selection(0, 0));
        const auto& u{w[selection(0, 0)]};
        if (u.productionQueue)
          drawProductionQueue(ios, w, rts::StableRef{u});
        if (auto max{w[u.type].maxEnergy}) {
          graph::setColor(win, Color::Magenta);
          mvwprintw(win.w, 6, 58, "%3d/%3d", u.energy, max);
        }
      }
    }

    void drawAbilities(
        const IOState& ios,
        const rts::World& w,
        const Player& player,
        const rts::Selection::Subgroup& subgroup) {
      const auto& win{ios.controlWin};

      auto readyState{subgroup.abilityReadyState(w)};

      for (int row{0}; row < 3; ++row) {
        for (int col{0}; col < 5; ++col) {
          rts::AbilityInstanceIndex ai(
              player.abilityPage * rts::MaxUnitAbilitiesPerPage + row * 5 + col);
          assert(ai < rts::MaxUnitAbilities);
          const auto rs{readyState[ai]};
          if (rs == rts::AbilityReadyState::None)
            continue;
          if (const auto a{subgroup.abilities[ai]->abilityId}) {
            const bool ready{rs == rts::AbilityReadyState::Ready};
            ScreenRect rect{
                {79 + col * (dim::CellSizeEx.x + 1), 2 + row * dim::CellSizeEx.y},
                dim::CellSize + ScreenVector{1, 0}};
            graph::setColor(win, Color::BrightGreen);
            graph::drawRect(win, boundingBox(rect));
            wattr_set(
                win.w,
                (player.selectingAbilityTarget && ai == player.selectingAbilityTarget->abilityIndex)
                    ? A_BOLD
                    : 0,
                graph::colorPair(ready ? Color::White : Color::DarkGray), nullptr);
            mvwaddch(win.w, rect.topLeft.y, rect.topLeft.x, ui::Layout::abilityKey(ai));
            graph::drawFrame(
                win, getIcon(w[a]).frame(), rect.topLeft + rts::Vector{1, 0},
                ready ? Color::Green : Color::DarkGray);
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
        graph::setColor(win, Color::Red);
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

void ui::Output::onGameStart(const rts::World& w) {
  uiMinimap.init(w);
}

void ui::Output::update(const rts::Engine& engine, const rts::World& w, Player& player) {
  player.update(w);
  doUpdate(engine, w, player);
}

void ui::Output::doUpdate(const rts::Engine& engine, const rts::World& w, const Player& player) {
  const auto& camera{player.camera};
  const auto& side{w[player.side]};

  if (ios_.redraw || termResized) {
    ios_.redraw = termResized = false;
    redraw(ios_);
  }

  for (const Window* win : allWins)
    werase(win->w);

  grid(ios_.renderWin);

#ifdef MAP_DEBUG
  mapDebug(ios_.renderWin, w, camera);
#endif

  const auto& subgroup{side.selection().subgroup(w)};

  rts::Rectangle prototypeArea;
  if (side.prototype()) {
    const auto& proto{w[side.prototype()]};
    prototypeArea = rectangleCenteredAt(ios_.mouseMapPoint, proto.area.size, w.map.area());
    drawPower(ios_, camera, side.powerMap());
    if (auto pr{w[proto.type].powerRadius})
      drawPower(ios_, camera, prototypeArea, pr, graph::colorPair(Color::ElectricBlue1));
    if (proto.type == side.baseType())
      drawResourceProximity(ios_, camera, w.resourceProximityMap);
  }
  else if (subgroup.type) {
    if (auto pr{w[subgroup.type].powerRadius})
      drawPower(ios_, w, camera, subgroup, pr);
  }
  highlight(ios_.renderWin, camera, ios_.targetPoint, graph::colorPair(Color::Red));
  render(ios_.renderWin, w, {w.map, side.prototypeMap()}, camera, side.selection());
  if (player.selectionBox)
    drawBoundingBox(ios_.renderWin, camera, *player.selectionBox, graph::colorPair(Color::Green));
  if (side.prototype())
    render(ios_.renderWin, w, camera, w[side.prototype()], prototypeArea);
  else if (player.selectingAbilityTarget)
    drawTargetSelectPoint(ios_, w, player, subgroup, player.selectingAbilityTarget->abilityIndex);

  drawResourceQuantities(ios_, w, side);
  drawGameTime(ios_, engine, w);
  drawGameSpeed(ios_, engine);
  drawFps(ios_, engine);
  drawControlGroups(ios_, w, side);

  drawSelection(ios_, w, player.worldView, subgroup.type);
  drawAbilities(ios_, w, player, subgroup);
  drawMessages(ios_, w, side.messages());

  uiMinimap.update(ios_.controlWin, w, camera, player.side);

  if (termTooSmall) {
    werase(ios_.headerWin.w);
    mvwprintw(
        ios_.headerWin.w, 0, 0,
        "=== PLEASE RESIZE TERMINAL TO AT LEAST %d LINES AND %d COLUMNS (press Q to quit) ===",
        dim::TotalSize.y, dim::TotalSize.x);
    if (!ios_.paused()) {
      ios_.menu.show();
      X::releaseInput();
    }
  }
  else if (ios_.paused()) {
    MenuImpl::print(ios_.menu, ios_.headerWin);
    mvwprintw(ios_.headerWin.w, 0, 40, "=== GAME PAUSED ===");
  }
  else {
    mvwprintw(ios_.headerWin.w, 0, 0, "F10:menu");
#ifdef MAP_DEBUG
    mvwprintw(
        ios_.headerWin.w, 0, 32, "(%d, %d) - (%d, %d) : (%d, %d)", camera.topLeft().x,
        camera.topLeft().y, camera.bottomRight().x, camera.bottomRight().y, ios_.mouseMapPoint.x,
        ios_.mouseMapPoint.y);
#else
    mvwprintw(ios_.headerWin.w, 0, 40, "=== SC2TOO ===");
#endif
  }

  for (const Window* win : allWins)
    wnoutrefresh(win->w);
  doupdate();
}
