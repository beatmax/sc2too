#include "ui/fx.h"

#include "Frame.h"
#include "graph.h"

#include <map>
#include <numeric>

namespace {
  using CP = std::pair<int, int>;

  const std::array<int, 256> UnpowerColorTr = [] {
    std::array<int, 256> t;
    std::iota(t.begin(), t.end(), 0);
    for (auto [k, v] : {CP{51, 6}, CP{208, 136}, CP{226, 142}})
      t[k] = v;
    return t;
  }();

  inline int unpowerColorTr(int c) { return (c & ~0xff) ? c : UnpowerColorTr[c]; }
}

ui::Sprite ui::fx::flatten(const Sprite& s) {
  auto fp{std::make_unique<Frame>(s.frame())};
  auto& frame{*fp};

  util::Matrix<int, int> isSpace(frame.rows(), frame.cols());
  for (int y{0}; y < frame.rows(); ++y) {
    for (int x{0}; x < frame.cols(); ++x) {
      wchar_t wch[2];
      attr_t attrs;
      short colorPair;
      getcchar(&frame(y, x), wch, &attrs, &colorPair, nullptr);
      isSpace(y, x) = (wch[0] == L' ');
    }
  }

  auto atBorder = [&](int y, int x) {
    return (y == 0 || x == 0 || y == (frame.rows() - 1) || x == (frame.cols() - 1));
  };

  util::Matrix<int, int> isEdge(frame.rows(), frame.cols());
  for (int y{0}; y < frame.rows(); ++y) {
    for (int x{0}; x < frame.cols(); ++x) {
      isEdge(y, x) = !isSpace(y, x) &&
          (atBorder(y, x) || isSpace(y - 1, x) || isSpace(y, x - 1) || isSpace(y, x + 1) ||
           isSpace(y + 1, x));
    }
  }

  util::Matrix<int, int> isInnerEdge(frame.rows(), frame.cols());
  for (int y{0}; y < frame.rows(); ++y) {
    for (int x{0}; x < frame.cols(); ++x) {
      isInnerEdge(y, x) = !isSpace(y, x) && !isEdge(y, x) &&
          (isEdge(y - 1, x - 1) || isEdge(y - 1, x) || isEdge(y - 1, x + 1) || isEdge(y, x - 1) ||
           isEdge(y, x + 1) || isEdge(y + 1, x - 1) || isEdge(y + 1, x) || isEdge(y + 1, x + 1));
    }
  }

  for (int y{0}; y < frame.rows(); ++y) {
    for (int x{0}; x < frame.cols(); ++x) {
      wchar_t wch[2];
      attr_t attrs;
      short colorPair;
      getcchar(&frame(y, x), wch, &attrs, &colorPair, nullptr);
      short fg, bg;
      pair_content(colorPair, &fg, &bg);
      if (isEdge(y, x))
        colorPair = graph::colorPair(159, (bg == -1) ? -1 : 159);
      else if (isInnerEdge(y, x))
        colorPair = graph::colorPair(123, (bg == -1) ? -1 : 123);
      else if (!isSpace(y, x))
        colorPair = graph::colorPair(87, (bg == -1) ? -1 : 87);
      setcchar(&frame(y, x), wch, attrs, colorPair, nullptr);
    }
  }

  return Sprite{std::move(fp)};
}

ui::Sprite ui::fx::unpower(const Sprite& s) {
  auto fp{std::make_unique<Frame>(s.frame())};
  auto& frame{*fp};

  for (int y{0}; y < frame.rows(); ++y) {
    for (int x{0}; x < frame.cols(); ++x) {
      wchar_t wch[2];
      attr_t attrs;
      short colorPair;
      getcchar(&frame(y, x), wch, &attrs, &colorPair, nullptr);
      if (colorPair) {
        short fg, bg;
        pair_content(colorPair, &fg, &bg);
        colorPair = graph::colorPair(unpowerColorTr(fg), unpowerColorTr(bg));
        setcchar(&frame(y, x), wch, attrs, colorPair, nullptr);
      }
    }
  }

  frame.setDefaultBg(Color(unpowerColorTr(int(frame.defaultBg()))));

  return Sprite{std::move(fp)};
}
