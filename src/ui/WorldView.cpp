#include "ui/WorldView.h"

#include "rts/Side.h"

void ui::WorldView::update(const rts::World& w, const rts::Side& side) {
  updateSelection(w, side.selection());
}

void ui::WorldView::updateSelection(const rts::World& w, const rts::Selection& s) {
  const auto& ids{s.ids(w)};
  selection.setRaw(ids);
  selectionTotalSize = ids.size();
}
