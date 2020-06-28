#pragma once

#include "rts/Blocker.h"
#include "rts/Entity.h"
#include "rts/Map.h"
#include "rts/Resource.h"
#include "rts/ResourceField.h"
#include "rts/Side.h"
#include "rts/types.h"

#include <map>
#include <string>
#include <vector>

namespace test {
  extern const std::string map;

  std::vector<rts::Side> makeSides();

  struct Ui : rts::Ui {
    static std::map<char, int> count;
    char repr;

    explicit Ui(char r) : repr{r} { ++count[repr]; }
    ~Ui() override { --count[repr]; }
  };

  inline char repr(const rts::Ui& ui) { return static_cast<const Ui&>(ui).repr; }
  inline char repr(const rts::UiUPtr& ui) { return static_cast<const Ui&>(*ui).repr; }

  extern const rts::Resource gas;

  struct Factory {
    static rts::Entity simpleton(rts::Point p, rts::SideCPtr sd);
    static rts::Entity building(rts::Point p, rts::SideCPtr sd);
    static rts::ResourceField geyser(rts::Point p);
    static rts::Blocker rock(rts::Point p);
  };

  class MapInitializer : public rts::MapInitializer {
  public:
    void operator()(rts::World& world, rts::Point p, char c) const final;
  };
}
