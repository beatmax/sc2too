#pragma once

#include "Camera.h"
#include "types.h"

#include <sys/types.h>

namespace ui::dim {
  constexpr ScreenVector CellSize{3, 1};
  constexpr ScreenVector CellSizeEx{CellSize + ScreenVector{1, 1}};

  constexpr ScreenVector MapViewSize{scale(Camera::Size, CellSizeEx) - ScreenVector{1, 1}};
  constexpr ScreenRect MapViewArea{{0, 0}, MapViewSize};

  constexpr ScreenCoordinate DefaultWinLeft{1};
  constexpr ScreenCoordinate DefaultWinWidth{MapViewSize.x};

  constexpr ScreenCoordinate HeaderWinTop{1};
  constexpr ScreenCoordinate HeaderWinHeight{1};

  constexpr ScreenCoordinate RenderWinTop{HeaderWinTop + HeaderWinHeight + 1};
  constexpr ScreenCoordinate RenderWinHeight{MapViewSize.y};

  constexpr ScreenCoordinate ControlWinTop{RenderWinTop + RenderWinHeight + 1};
  constexpr ScreenCoordinate ControlWinHeight{9};

  constexpr ScreenVector TotalSize{DefaultWinWidth + 2, ControlWinTop + ControlWinHeight + 1};

  constexpr MatrixVector SelectionMatrixSize{8, 3};
  constexpr ScreenRect SelectionArea{{40, 2}, scale(SelectionMatrixSize, CellSizeEx)};
  constexpr ScreenRect SelectionAreaEx{
      SelectionArea.topLeft - ScreenVector{1, 1}, SelectionArea.size + ScreenVector{1, 1}};

  constexpr ScreenRect MinimapArea{{0, 0}, {18, 9}};
}
