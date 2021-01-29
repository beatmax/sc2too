#include "XPixelTr.h"

#include "X.h"
#include "graph.h"
#include "util/ScopeExit.h"
#include "util/algorithm.h"

#include <cassert>
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
      colorMap[0] = '-';
      auto cm = [&](unsigned long raw) {
        if (!colorMap.count(raw))
          colorMap[raw] = colorMap.size() <= ('z' - '0') ? '0' + colorMap.size() : '~';
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
    using namespace std::chrono_literals;

    constexpr auto CandidateMinTime{100ms};
    constexpr auto CalibStartDelay{100ms};
    constexpr auto CalibRetryTime{1ms};
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

void ui::XPixelTr::update(const Sample& p) {
  if (auto target{filter(p)}) {
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
  candidate_.reset();
}

// Filter to detect good samples for calibration and calibration checking:
// There is a delay between X cursor (pixel point) movement and terminal mouse
// (screen point) movement events. We have a good match if the screen point
// stays the same during that delay. We assume the delay is lower than
// CandidateMinTime. Also, make sure the cursor stays inside the terminal
// window, which is achieved by checking that the move direction is stable and
// the screen point moves to an adjacent cell.
auto ui::XPixelTr::filter(const Sample& p) -> std::optional<Sample> {
  if (!candidate_) {
    candidate_ = p;
    ppPrev_ = p.pp;
    ppDirection_ = {0, 0};
    return std::nullopt;
  }

  if (!stableDirection(p.pp)) {
    DEB(util::trace() << "[XPixelTr] Rejecting candidate, direction not stable." << std::endl;)
    goto reject;
  }

  if (p.sp == candidate_->sp)
    return std::nullopt;

  if ((p.tp - candidate_->tp) < CandidateMinTime) {
    DEB(util::trace() << "[XPixelTr] Rejecting candidate, insufficient time." << std::endl;)
    goto reject;
  }

  if (!adjacent(p.sp, candidate_->sp)) {
    DEB(util::trace() << "[XPixelTr] Rejecting candidate, destination not adjacent." << std::endl;)
    goto reject;
  }

  ppPrev_ = candidate_->pp;
  ppDirection_ = unit(p.sp - candidate_->sp);
  if (!stableDirection(p.pp)) {
    DEB(util::trace() << "[XPixelTr] Rejecting candidate, sp/pp direction mismatch." << std::endl;)
    goto reject;
  }

  return std::exchange(candidate_, std::nullopt);

reject:
  candidate_.reset();
  return std::nullopt;
}

bool ui::XPixelTr::stableDirection(PixelPoint pp) {
  auto dir{unit(pp - std::exchange(ppPrev_, pp))};

  auto checkUpdate = [&](auto coord) {
    if (!coord(dir))
      return true;
    if (!coord(ppDirection_)) {
      coord(ppDirection_) = coord(dir);
      return true;
    }
    return coord(dir) == coord(ppDirection_);
  };

  bool okX{checkUpdate([](PixelVector& pv) -> PixelCoordinate& { return pv.x; })};
  bool okY{checkUpdate([](PixelVector& pv) -> PixelCoordinate& { return pv.y; })};
  return okX && okY;
}

void ui::XPixelTr::calibrate(const Sample& target) {
  DEB(util::trace() << "[XPixelTr] Calibration target: " << target.sp << " " << target.pp
                    << std::endl;)

  cchar_t savedChars[2];
  clear(target.sp, savedChars);
  util::ScopeExit restoreChars{[&] { restore(target.sp, savedChars); }};

  // it takes some time until the X display is updated; the terminal may still
  // be drawing previous frames, and there is no easy way to determine when
  // that's done; this delay may be longer than needed to be on the safe side
  std::this_thread::sleep_for(CalibStartDelay);

  const PixelRect calibArea{rectangleCenteredAt(
      target.pp, CalibAreaSize,
      {{0, 0}, {PixelCoordinate(X::displayWidth), PixelCoordinate(X::displayHeight)}})};
  auto relTarget{toPoint(target.pp - calibArea.topLeft)};

  const X::PixelMatrix img0{X::getImage(calibArea)};
  DEB(util::trace() << "[XPixelTr] Calib initial img: " << std::endl; print(img0, relTarget);)

  paint(target.sp, Color::Gray);
  int tries{CalibMaxTries};
  X::PixelMatrix img1;
  bool eq;
  do {
    std::this_thread::sleep_for(CalibRetryTime);
    img1 = X::getImage(calibArea);
    eq = (img0 == img1);
  } while (--tries && eq);
  DEB(util::trace() << "[XPixelTr] Target color " << img0[relTarget] << " -> " << img1[relTarget]
                    << " after " << (CalibMaxTries - tries) << " tries" << std::endl;)
  if (eq) {
    DEB(util::trace() << "[XPixelTr] Calibration failed: no color change" << std::endl;)
    return;
  }

  const Matrix diff{img1 - img0};

  if (!diff[relTarget]) {
    // target wasn't precise; sometimes the terminal doesn't generate a mouse
    // movement event when the X cursor is just at the border of the cell
    auto it = util::findIf(diff.rect().points(), [&](MatrixPoint p) { return bool(diff[p]); });
    assert(it != diff.rect().points().end());
    DEB(util::trace() << "[XPixelTr] Changing target: " << relTarget << " -> " << *it << std::endl);
    relTarget = *it;
  }

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

bool ui::XPixelTr::checkCalibration(const Sample& target) {
  bool match{toSubcharPoint(target.pp)->point == target.sp};
  if (!match) {
    // see comment in calibrate() about imprecise target
    match = util::anyOf(boundingBox(target.pp).outerPoints(), [&](PixelPoint p) {
      return toSubcharPoint(p)->point == target.sp;
    });
    DEB(if (match) util::trace() << "[XPixelTr] Accepting off-by-one target." << std::endl;)
  }
  DEB(util::trace() << "[XPixelTr] " << (match ? "Match" : "MISMATCH") << ": sp=" << target.sp
                    << " pp=" << target.pp << std::endl;)
  return match;
}
