#pragma once

#include "rts/Command.h"
#include "rts/types.h"

#include <ostream>
#include <utility>
#include <vector>

namespace test {
  struct TestAccount {
    rts::Quantity available;
    rts::Quantity allocated;

    void provision(rts::Quantity q) { available += q; }
    void allocate(rts::Quantity q);
    void lose(rts::Quantity q) { allocated -= q; }
    void restore(rts::Quantity q) { allocate(-q); }
    bool operator==(const TestAccount& other) const;
  };

  struct TestResources {
    TestAccount gas{}, supply{};

    TestResources() = default;
    explicit TestResources(const rts::Side& s);
    bool operator==(const TestResources& other) const;
  };

  std::ostream& operator<<(std::ostream& os, const TestAccount& a);
  std::ostream& operator<<(std::ostream& os, const TestResources& r);

  using MoveStep = std::pair<rts::Point, rts::GameTime>;
  using MoveStepList = std::vector<MoveStep>;

  rts::GameTime nextStepTime(const rts::Unit& u);
  void stepUpdate(rts::World& w, const rts::Unit& u);
  void execCommand(rts::World& w, rts::SideId side, rts::Command command);
  void select(
      rts::World& w,
      rts::SideId side,
      rts::UnitIdList units,
      rts::command::Selection::Action action = rts::command::Selection::Set);
  MoveStepList runMove(
      rts::World& w, rts::Unit& unit, rts::Point target, rts::GameTime timeout = 99999);
  MoveStepList continueMove(rts::World& w, rts::Unit& unit, rts::GameTime timeout = 99999);
}
