#pragma once

#include "ui/Camera.h"

#include <sys/types.h>

namespace ui::dim {
  constexpr int cellWidth{3};
  constexpr int cellHeight{1};

  constexpr int totalCellWidth{int(Camera::width) * (1 + cellWidth) - 1};
  constexpr int totalCellHeight{int(Camera::height) * (1 + cellHeight) - 1};

  constexpr int defaultWinLeft{1};
  constexpr int defaultWinWidth{totalCellWidth};

  constexpr int headerWinTop{1};
  constexpr int headerWinHeight{1};

  constexpr int renderWinTop{headerWinTop + headerWinHeight + 1};
  constexpr int renderWinHeight{totalCellHeight};

  constexpr int controlWinTop{renderWinTop + renderWinHeight + 1};
  constexpr int controlWinHeight{9};

  constexpr int totalWidth{defaultWinWidth + 2};
  constexpr int totalHeight{controlWinTop + controlWinHeight + 1};
}
