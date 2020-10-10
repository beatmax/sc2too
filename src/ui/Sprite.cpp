#include "ui/Sprite.h"

#include "Frame.h"
#include "dim.h"
#include "graph.h"
#include "rts/World.h"
#include "util/algorithm.h"
#include "util/fs.h"

#include <cassert>
#include <cstdlib>
#include <cwchar>
#include <limits>
#include <map>
#include <sstream>
#include <utility>

namespace {
  using LineColors = std::vector<std::pair<int, int>>;
  using Parts = std::pair<std::vector<std::vector<std::wstring>>, std::vector<LineColors>>;

  constexpr std::pair<int, int> defaultColor{15, -1};

  Parts parse(std::vector<std::wstring>&& lines, rts::GameTime& frameDuration, bool& directional) {
    Parts result;
    auto& [frames, colors] = result;

    frameDuration = 0;
    directional = false;

    auto it = lines.begin();

    if (it != lines.end() && *it == L"@DIR") {
      directional = true;
      ++it;
    }

    if (it != lines.end() && it->rfind(L"@T:", 0) == 0) {
      frameDuration = std::wcstol(it->c_str() + 3, nullptr, 10);
      ++it;
    }

    size_t frameHeight{std::numeric_limits<size_t>::max()};
    if (it != lines.end() && it->rfind(L"@H:", 0) == 0) {
      frameHeight = std::wcstol(it->c_str() + 3, nullptr, 10);
      ++it;
    }

    while (it != lines.end() && !it->empty()) {
      auto& chars{frames.emplace_back()};
      for (size_t n{0}; n < frameHeight && it != lines.end() && !it->empty(); ++n, ++it)
        chars.push_back(std::move(*it));
      assert(chars.size() == frames[0].size());
    }

    if (it != lines.end()) {
      assert(it->empty());
      ++it;
    }

    std::vector<std::wstring> colorLines;
    if (it != lines.end() && *it == L"=") {
      assert(frames.size() > 0);
      for (auto& chars : frames)
        colorLines.insert(colorLines.end(), chars.begin(), chars.end());
      ++it;
      assert(it == lines.end() || it->empty());
    }
    else {
      for (; it != lines.end() && !it->empty(); ++it)
        colorLines.push_back(std::move(*it));
    }

    if (it != lines.end())
      ++it;

    std::map<wchar_t, std::pair<int, int>> colorMap;
    for (int n = 0; n <= 9; ++n)
      colorMap[L'0' + n] = {n, -1};
    for (; it != lines.end(); ++it) {
      const std::wstring& mapLine{*it};
      assert(mapLine.size() > 2 && mapLine[1] == L':');
      int fg = std::wcstol(mapLine.c_str() + 2, nullptr, 10);
      auto bgPos = mapLine.find(',');
      int bg =
          (bgPos == std::string::npos) ? -1 : std::wcstol(mapLine.c_str() + bgPos + 1, nullptr, 10);
      colorMap[mapLine[0]] = {fg, bg};
    }

    for (it = colorLines.begin(); it != colorLines.end(); ++it) {
      LineColors lineColors{util::transform(*it, [&](wchar_t c) {
        auto itMap = colorMap.find(c);
        return itMap != colorMap.end() ? itMap->second : defaultColor;
      })};
      colors.push_back(std::move(lineColors));
    }

    return result;
  }
}

int ui::detail::defaultDefaultColor() {
  return graph::white();
}

ui::Sprite::Sprite() = default;

ui::Sprite::Sprite(const std::wstring& s) : Sprite{std::wistringstream{s}} {
}

ui::Sprite::Sprite(std::wistream&& is) : Sprite{util::fs::readLines(is)} {
}

ui::Sprite::Sprite(std::vector<std::wstring>&& lines) {
  const auto [charFrames, colors] = parse(std::move(lines), frameDuration_, directional_);
  auto lineColors{colors.begin()};
  for (auto& chars : charFrames) {
    auto& frame{*frames_.emplace_back(
        std::make_unique<Frame>(chars.size(), chars.empty() ? 0 : chars.front().size()))};
    for (int y = 0; y < frame.rows(); ++y) {
      const auto& line = chars[y];
      assert(line.size() == size_t(frame.cols()));
      for (int x = 0; x < frame.cols(); ++x) {
        wchar_t wch[]{line[x], L'\0'};
        const auto* color = (lineColors != colors.end() && x < int(lineColors->size()))
            ? &(*lineColors)[x]
            : nullptr;
        const int colorPair = color ? graph::colorPair(color->first, color->second) : 0;
        setcchar(&frame(y, x), wch, 0, colorPair, nullptr);
      }
      if (lineColors != colors.end())
        ++lineColors;
    }
  }
  frameCount_ = directional_ ? frames_.size() / 4 : frames_.size();
}

ui::Sprite::Sprite(std::unique_ptr<Frame> frame) {
  frames_.push_back(std::move(frame));
  frameCount_ = 1;
}

ui::Sprite::~Sprite() = default;

const ui::Frame& ui::Sprite::frame(FrameIndex frame, rts::Direction direction) const {
  if (directional_)
    frame += uint32_t(direction) * frameCount();
  assert(frame < frames_.size());
  return *frames_[frame];
}

rts::Rectangle ui::Sprite::area(rts::Point topLeft) const {
  return {
      topLeft,
      {rts::Coordinate((frame().cols() + 1) / (dim::cellWidth + 1)),
       rts::Coordinate((frame().rows() + 1) / (dim::cellHeight + 1))}};
}

void ui::SpriteUiBase::update(const rts::World& w, const rts::WorldObject& obj) {
  auto* prev{sprite_};
  sprite_ = &spriteProtected(w, obj);
  if (sprite_ != prev) {
    frameIndex_ = 0;
    nextFrameTime_ =
        sprite_->frameDuration() ? w.time + sprite_->frameDuration() : rts::GameTimeInf;
  }
  else if (w.time >= nextFrameTime_) {
    assert(sprite_->frameCount() > 0);
    assert(sprite_->frameDuration() > 0);
    ldiv_t d{ldiv(w.time - nextFrameTime_, sprite_->frameDuration())};
    frameIndex_ = (frameIndex_ + 1 + d.quot) % sprite_->frameCount();
    nextFrameTime_ = w.time + sprite_->frameDuration() - d.rem;
  }
}
