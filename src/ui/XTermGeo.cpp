#include "XTermGeo.h"

#include "X.h"
#include "graph.h"
#include "util/ScopeExit.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <optional>
#include <random>
#include <thread>
#include <utility>
#include <vector>

// conflicting macros
#include "Xdetail.h"

#include <X11/Xutil.h>

//#define XTERMGEO_DEBUG

#ifdef XTERMGEO_DEBUG
#include "util/debug.h"
#define DEB(x) x
#else
#define DEB(x)
#endif

namespace ui::loc {
  inline namespace {
    using namespace std::chrono_literals;

    using Pattern = std::vector<int>;
    using PatternArray = std::array<Pattern, 3>;
    using PixelLine = std::vector<unsigned long>;
    using Sequence = std::vector<int>;

    constexpr auto CalibRetryDelay{100ms};
    constexpr auto ScanInitialDelay{50ms};
    constexpr auto ScanRetryDelay{10ms};
    constexpr auto ScanMaxRetries{5};
    constexpr PixelVector ScanInitialAreaSize{400, 200};

    const std::vector<int> BaseCalibPattern{1, 1, 2, 2, 3, 3, 4};
    const int CalibPatternLength{
        std::accumulate(BaseCalibPattern.begin(), BaseCalibPattern.end(), 0)};

    void generate(PatternArray& patterns);
    void draw(const PatternArray& patterns);
    std::optional<PixelRect> scan(const PixelRect& area, const PatternArray& patterns);
    Sequence::const_iterator detect(const Pattern& pattern, const Sequence& seq);
    Sequence sequence(const PixelLine& pixels);
    PixelLine pixelLine(XImage* img, int y);
  }
}

void ui::XTermGeo::update() {
  if (outdated()) {
    terminalRect_ = X::detail::activeWindowRect;
    while (!calibrate())
      std::this_thread::sleep_for(loc::CalibRetryDelay);
  }
}

bool ui::XTermGeo::outdated() {
  X::detail::updateActiveWindowRect();
  return X::detail::activeWindowRect.size != terminalRect_.size;
}

ui::SubcharPoint ui::XTermGeo::toSubcharPoint(PixelPoint pp) const {
  auto p{pp - calibration_.base};
  auto dx{std::div(p.x, calibration_.charSize.x)};
  auto dy{std::div(p.y, calibration_.charSize.y)};
  return SubcharPoint{
      {ScreenCoordinate(dx.quot), ScreenCoordinate(dy.quot)},
      fscale(PixelVector{dx.rem, dy.rem}, calibration_.factor)};
}

bool ui::XTermGeo::calibrate() {
  DEB(util::trace() << "[XTermGeo] Calibrate, terminalRect: " << terminalRect_ << std::endl;)

  util::ScopeExit restoreScreen{[ch{getbkgd(stdscr)}] {
    attrset(0);
    bkgdset(ch);
    clear();
  }};
  endwin();
  bkgdset(graph::colorAttr(Color::White, Color::Black) | ' ');
  refresh();
  clear();

  loc::PatternArray patterns;
  loc::generate(patterns);

  {
    auto ssz{graph::screenSize()};
    ScreenVector minsz{loc::CalibPatternLength + 2, patterns.size()};
    if (ssz.x < minsz.x || ssz.y < minsz.y)
      return false;
  }

  loc::draw(patterns);

  // it takes some time until the X display is updated
  std::this_thread::sleep_for(loc::ScanInitialDelay);

  for (int tries{0}; tries < loc::ScanMaxRetries; ++tries) {
    if (auto visibleTermArea{maybeIntersection(terminalRect_, {{0, 0}, X::displaySize()})}) {
      const PixelRect area{
          toPoint(visibleTermArea->topLeft - terminalRect_.topLeft),
          min((tries + 1) * loc::ScanInitialAreaSize, visibleTermArea->size)};

      if (auto charArea{loc::scan(area, patterns)}) {
        calibration_ = {
            area.topLeft + toVector(charArea->topLeft) - scale(ScreenVector{1, 0}, charArea->size),
            charArea->size, relation({1, 1}, charArea->size)};
        DEB(util::trace() << "[XTermGeo] Calibrated! base=" << calibration_.base
                          << " charSize=" << calibration_.charSize << std::endl;)
        return true;
      }
    }

    DEB(util::trace() << "[XTermGeo] Scan failed..." << std::endl;)
    std::this_thread::sleep_for(loc::ScanRetryDelay);

    // maybe we were looking at the wrong window
    X::detail::updateActiveWindow();
    terminalRect_ = X::detail::activeWindowRect;
  }
  DEB(util::trace() << "[XTermGeo] Calibration failed." << std::endl;)
  return false;
}

void ui::loc::generate(PatternArray& patterns) {
  std::random_device rd;
  std::mt19937 g(rd());

  Pattern prev;
  for (auto& p : patterns) {
    p = BaseCalibPattern;
    do {
      std::shuffle(p.begin(), p.end(), g);
    } while (p == prev);
    prev = p;
  }
}

void ui::loc::draw(const PatternArray& patterns) {
  Color color[]{Color::White, Color::Black};
  move(0, 0);
  int y{0};
  for (auto& p : patterns) {
    move(y++, 1);
    int ci{0};
    for (int n : p) {
      attrset(graph::colorAttr(color[ci], color[ci]));
      while (n--)
        addch(' ');
      ci = !ci;
    }
  }
  refresh();
}

std::optional<ui::PixelRect> ui::loc::scan(const PixelRect& area, const PatternArray& patterns) {
  size_t pat{0};
  int cnt{0}, height{0}, x{0}, f{0};

  X::detail::trapErrors();
  XImage* img{XGetImage(
      X::detail::display, X::detail::activeWindow, area.topLeft.x, area.topLeft.y, area.size.x,
      area.size.y, AllPlanes, XYPixmap)};
  auto ec{X::detail::untrapErrors()};
  if (ec) {
    DEB(util::trace() << "[XTermGeo] XGetImage failed: " << ec << std::endl;)
    return std::nullopt;
  }

  for (int y{0}; y < img->height; ++y) {
    const auto& pattern{patterns[pat]};
    const auto seq{sequence(pixelLine(img, y))};

    if (auto match{detect(pattern, seq)}; match != seq.end()) {
      auto xx{std::accumulate(seq.begin(), match, 0)};
      auto ff{*match / pattern.front()};
      DEB(util::trace() << "[XTermGeo] Pattern " << pat << " detected: x=" << xx << " y=" << y
                        << " f=" << ff;)

      if (x == xx && f == ff)
        ++cnt;
      else if (pat == 0)
        cnt = 1, x = xx, f = ff;
      else
        cnt = 0, pat = 0;

      DEB(util::trace() << " cnt=" << cnt << std::endl;)

      if (pat == 2) {
        XFree(img);
        return PixelRect{{x, y - height * 2}, {f, height}};
      }
    }
    else if (cnt) {
      ++pat, height = cnt, cnt = 0, --y;
    }
    else {
      pat = 0;
    }
  }
  XFree(img);
  return std::nullopt;
}

ui::loc::Sequence::const_iterator ui::loc::detect(const Pattern& pattern, const Sequence& seq) {
  if (pattern.empty())
    return seq.begin();
  if (seq.size() < pattern.size())
    return seq.end();
  const auto noMatch{seq.end() - pattern.size() + 1};
  for (auto start{seq.begin()}; start != noMatch; ++start) {
    if (auto q{div(*start, pattern.front())}; !q.rem) {
      const auto factor{q.quot};
      auto itPat{pattern.begin() + 1};
      for (auto itSeq{start + 1}; itPat != pattern.end() && *itSeq == (factor * *itPat);
           ++itPat, ++itSeq)
        ;
      if (itPat == pattern.end())
        return start;
    }
  }
  return seq.end();
}

ui::loc::Sequence ui::loc::sequence(const PixelLine& pixels) {
  Sequence seq;
  auto it{pixels.begin()};
  while (it != pixels.end()) {
    auto next{std::find_if(it + 1, pixels.end(), [p{*it}](auto q) { return p != q; })};
    seq.push_back(std::distance(it, next));
    it = next;
  }
  return seq;
}

ui::loc::PixelLine ui::loc::pixelLine(XImage* img, int y) {
  PixelLine line;
  line.reserve(img->width);
  for (int x{0}; x < img->width; ++x)
    line.push_back(XGetPixel(img, x, y));
  return line;
}
