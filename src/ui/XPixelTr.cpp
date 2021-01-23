#include "XPixelTr.h"

#include "X.h"
#include "graph.h"
#include "util/ScopeExit.h"
#include "util/algorithm.h"

#include <chrono>
#include <cstdlib>
#include <thread>
#include <utility>

//#define XPIXELTR_DEBUG

#ifdef XPIXELTR_DEBUG
#include "util/debug.h"

#define DEB(x) x

namespace ui {
  namespace {
    void print(const X::PixelMatrix& m, MatrixPoint relTarget) {
      util::trace() << "-------------------" << std::endl;
      std::map<unsigned long, char> colorMap;
      auto cm = [&](unsigned long raw) {
        if (!raw)
          return '-';
        if (!colorMap.count(raw))
          colorMap[raw] = '0' + colorMap.size();
        return colorMap[raw];
      };
      for (auto p : m.rect().points()) {
        util::trace() << (p == relTarget ? '*' : cm(m[p]));
        if (p.x == m.cols() - 1)
          util::trace() << std::endl;
      }
    }
  }
}
#else
#define DEB(x)
#endif

namespace ui {
  namespace {
    constexpr int CalibStartDelayMs{100};
    constexpr int CalibRetryMs{1};
    constexpr int CalibMaxTries{100};
    constexpr PixelVector CalibAreaSize{50, 100};  // determines max detectable char size

    void paint(ScreenPoint p, Color c) {
      attrset(graph::colorAttr(c, c));
      mvaddch(p.y, p.x, ' ');
      refresh();
    }

    void clear(ScreenPoint p, cchar_t saved[2]) {
      attrset(graph::colorAttr(Color::Black, Color::Black));
      if (p.x > 0) {
        // a previous large character may overlap
        mvin_wch(p.y, p.x - 1, &saved[0]);
        mvaddch(p.y, p.x - 1, ' ');
      }
      mvin_wch(p.y, p.x, &saved[1]);
      mvaddch(p.y, p.x, ' ');
      refresh();
    }

    void restore(ScreenPoint p, const cchar_t saved[2]) {
      attrset(0);
      if (p.x > 0)
        mvadd_wch(p.y, p.x - 1, &saved[0]);
      mvadd_wch(p.y, p.x, &saved[1]);
      refresh();
    }
  }
}

std::optional<ui::SubcharPoint> ui::XPixelTr::toSubcharPoint(PixelPoint pp) const {
  if (calibration_) {
    auto p{pp - calibration_->base};
    auto dx{std::div(p.x, calibration_->charSize.x)};
    auto dy{std::div(p.y, calibration_->charSize.y)};
    scale(PixelVector{dx.rem, dy.rem}, calibration_->factor);
    return SubcharPoint{
        {ScreenCoordinate(dx.quot), ScreenCoordinate(dy.quot)},
        fscale(PixelVector{dx.rem, dy.rem}, calibration_->factor)};
  }
  return std::nullopt;
}

void ui::XPixelTr::update(const PointPair& p) {
  if (auto* target{filter(p)}) {
    if (calibration_) {
      if (!checkCalibration(*target))
        reset();
    }
    else {
      calibrate(*target);
    }
  }
}

void ui::XPixelTr::reset() {
  calibration_.reset();
  prevSize_ = 0;
}

// Filter to detect good point pairs for calibration: if the last two pairs
// correspond to the same ncurses cell (screen point), and the direction of the
// X cursor (pixel point) was stable, there is a very high chance that the
// previous pair was a real match.
auto ui::XPixelTr::filter(const PointPair& p) -> const PointPair* {
  bool accept{false};
  if (prevSize_ < 2) {
    ++prevSize_;
  }
  else if (p.sp == prev_[0].sp) {
    auto stableDirection = [&](auto coord) {
      return (coord(p) >= coord(prev_[0]) && coord(prev_[0]) >= coord(prev_[1])) ||
          (coord(p) <= coord(prev_[0]) && coord(prev_[0]) <= coord(prev_[1]));
    };
    auto coordX = [](const PointPair& q) { return q.pp.x; };
    auto coordY = [](const PointPair& q) { return q.pp.y; };

    accept = stableDirection(coordX) && stableDirection(coordY);
  }

  prev_[1] = std::exchange(prev_[0], p);
  return accept ? &prev_[1] : nullptr;
}

void ui::XPixelTr::calibrate(const PointPair& target) {
  DEB(util::trace() << "[XPixelTr] Calibration target: " << target.sp << " " << target.pp
                    << std::endl;)

  cchar_t savedChars[2];
  clear(target.sp, savedChars);
  util::ScopeExit restoreChars{[&] { restore(target.sp, savedChars); }};

  // it takes some time until the X display is updated; the terminal may still
  // be drawing previous frames, and there is no easy way to determine when
  // that's done; this delay may be longer than needed to be on the safe side
  std::this_thread::sleep_for(std::chrono::milliseconds(CalibStartDelayMs));

  const PixelRect calibArea{rectangleCenteredAt(
      target.pp, CalibAreaSize,
      {{0, 0}, {PixelCoordinate(X::displayWidth), PixelCoordinate(X::displayHeight)}})};
  const auto relTarget{toPoint(target.pp - calibArea.topLeft)};

  const X::PixelMatrix img0{X::getImage(calibArea)};
  DEB(util::trace() << "[XPixelTr] Calib initial img: " << std::endl; print(img0, relTarget);)

  const auto color0{img0[relTarget]};
  auto color{color0};

  paint(target.sp, Color::Gray);
  int tries{CalibMaxTries};
  while (tries-- && color == color0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(CalibRetryMs));
    color = X::getPixel(target.pp);
  }
  DEB(util::trace() << "[XPixelTr] Target color " << color0 << " -> " << color << " after "
                    << (CalibMaxTries - tries) << " tries" << std::endl;)
  if (color == color0) {
    DEB(util::trace() << "[XPixelTr] Calibration failed: no color change" << std::endl;)
    return;
  }

  const X::PixelMatrix img1{X::getImage(calibArea)};
  const Matrix diff{img1 - img0};

  DEB(util::trace() << "[XPixelTr] Calib final img: " << std::endl; print(img1, relTarget);
      util::trace() << "[XPixelTr] Diff: " << std::endl; print(diff, relTarget);)

  auto topLeft{relTarget}, bottomRight{relTarget};
  while (topLeft.x >= 0 && diff[topLeft])
    --topLeft.x;
  ++topLeft.x;
  while (topLeft.y >= 0 && diff[topLeft])
    --topLeft.y;
  ++topLeft.y;
  while (bottomRight.x < diff.cols() && diff[bottomRight])
    ++bottomRight.x;
  --bottomRight.x;
  while (bottomRight.y < diff.rows() && diff[bottomRight])
    ++bottomRight.y;
  --bottomRight.y;

  const MatrixRect charArea{topLeft, bottomRight - topLeft + MatrixVector{1, 1}};
  if (!util::allOf(charArea.points(), [&](MatrixPoint p) { return bool(diff[p]); })) {
    DEB(util::trace() << "[XPixelTr] Calibration failed: inconsistent char area" << std::endl;)
    return;
  }

  calibration_ = {
      calibArea.topLeft + toVector(charArea.topLeft) - scale(toVector(target.sp), charArea.size),
      charArea.size, relation({1, 1}, charArea.size)};

  DEB(util::trace() << "[XPixelTr] Calibrated! base=" << calibration_->base
                    << " charSize=" << calibration_->charSize << std::endl;)
}

bool ui::XPixelTr::checkCalibration(const PointPair& target) {
  const bool match{toSubcharPoint(target.pp)->point == target.sp};
  DEB(if (!match) util::trace() << "[XPixelTr] Mismatch: sp=" << target.sp << " pp=" << target.pp
                                << std::endl;)
  return match;
}
