#pragma once

#include "dim.h"
#include "rts/types.h"

namespace ui {
  struct WorldView {
    Matrix<rts::UnitId> selection{dim::SelectionMatrixSize};
    size_t selectionTotalSize{};

    void update(const rts::World& w, const rts::Side& side);

  private:
    void updateSelection(const rts::World& w, const rts::Selection& s);
  };
}
