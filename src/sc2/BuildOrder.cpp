#include "sc2/BuildOrder.h"

#include "rts/World.h"
#include "sc2/UnitTypes.h"
#include "sc2/Upgrades.h"
#include "util/fs.h"

#include <fstream>
#include <stdexcept>

void sc2::BuildOrder::init(const rts::World& w) {
  auto f{std::ifstream("practice/build.txt")};
  if (!f)
    return;

  auto toId = [this](const auto& nameToId, const std::string& name) {
    if (auto it{nameToId.find(name)}; it != nameToId.end())
      return it->second;
    throw std::runtime_error{"bad name in build order: " + name};
  };

  auto buildLines = util::fs::readLines(f);
  for (const auto& line : buildLines) {
    if (line.substr(0, 8) == "ignored:")
      ignored_.emplace(toId(UnitTypes::nameToId, line.substr(8)));
    else if (line.substr(0, 9) == "research:")
      order_.emplace_back(toId(Upgrades::nameToId, line.substr(9)));
    else
      order_.emplace_back(toId(UnitTypes::nameToId, line));
  }
}

void sc2::BuildOrder::check(rts::World& w, rts::ProducibleId p, rts::SideId sd) {
  if (next_ >= order_.size() || ignored_.count(p))
    return;
  const auto& nextId = order_[next_];
  if (nextId == p) {
    ++next_;
    w[sd].messages().add(w, next_ < order_.size() ? "Good!" : "Excellent, that's it!");
  }
  else {
    std::string nextName;
    if (auto* utId{std::get_if<rts::UnitTypeId>(&nextId)})
      nextName = UnitTypes::idToName[*utId];
    else
      nextName = "research " + Upgrades::idToName[std::get<rts::UpgradeId>(nextId)];
    w[sd].messages().add(w, ("NOP!!! Next in build order: " + nextName).c_str());
  }
}
