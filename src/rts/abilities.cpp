#include "rts/abilities.h"

#include "Build.h"
#include "Gather.h"
#include "Move.h"
#include "Produce.h"
#include "SetRallyPoint.h"

int rts::abilities::mutualCancelGroup(Kind kind) {
  switch (kind) {
    case Kind::Build:
    case Kind::Gather:
    case Kind::Move:
      return 1;
    default:
      return 0;
  }
}

template<typename D>
rts::abilities::Instance rts::abilities::instance(const D& desc, AbilityStateIndex as) {
  return Instance{
      D::kind,
      desc.id,
      0,
      D::groupMode,
      D::targetType,
      D::enqueable,
      D::availableWhileBuilding,
      as,
      D::AbilityState::makeTrigger(desc),
      [desc]() -> const void* { return &desc; }};
}

rts::abilities::Instance rts::abilities::instance(const GoToPage& desc, AbilityStateIndex) {
  return Instance{GoToPage::kind, desc.id, desc.page};
}

namespace rts::abilities {
  template Instance instance<Build>(const Build&, AbilityStateIndex);
  template Instance instance<Gather>(const Gather&, AbilityStateIndex);
  template Instance instance<Move>(const Move&, AbilityStateIndex);
  template Instance instance<Produce>(const Produce&, AbilityStateIndex);
  template Instance instance<SetRallyPoint>(const SetRallyPoint&, AbilityStateIndex);
}
