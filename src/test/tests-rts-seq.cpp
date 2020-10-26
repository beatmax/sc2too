#include "tests-rts-seq.h"

#include "util/algorithm.h"

#include <algorithm>
#include <regex>
#include <utility>

namespace {
  const std::regex reCommaSep{"[^,]+"};

  std::vector<std::string> commaSplit(const std::string& s) {
    std::vector<std::string> r;
    std::transform(
        std::sregex_iterator(s.begin(), s.end(), reCommaSep), std::sregex_iterator(),
        std::back_inserter(r), [](const auto& m) { return m.str(); });
    return r;
  }

  std::string toString(rts::Point p) { return std::to_string(p.x) + ',' + std::to_string(p.y); }
}

namespace test::seq {
  namespace {
    struct ParseLoc {
      LineVector::const_iterator it;
      LineVector::const_iterator end;
    };
  }
}

namespace test::seq::item::cmd {
  namespace {
    std::string toString(const Select& c) { return "select " + util::join(c.names, ','); }
    std::string toString(const Trigger& c) {
      return "trigger " + c.ability + ' ' + ::toString(c.target);
    }
  }
}

namespace test::seq::item {
  namespace {
    std::string toString(const Error& e) { return e.text; }

    const std::regex reComment{R"(#(.*))"};
    Comment parseComment(const std::smatch& match) { return {match[1]}; }
    std::string toString(const Comment& c) { return '#' + c.text; }

    const std::regex reDefinition{R"((\w+)\s*:\s*(\w+))"};
    Definition parseDefinition(const std::smatch& match) { return {match[1], match[2]}; }
    std::string toString(const Definition& d) { return d.name + ": " + d.type; }

    const std::regex reAssignment{R"((\w+)\((\d+),(\d+)\)\s*=\s*(\d+))"};
    Assignment parseAssignment(const std::smatch& match) {
      rts::Point target{std::stoi(match[2]), std::stoi(match[3])};
      return {target, match[1], std::stoi(match[4])};
    }
    std::string toString(const Assignment& a) {
      return a.property + '(' + ::toString(a.target) + ") = " + std::to_string(a.value);
    }

    const std::regex reMap{R"(-+|=+)"};
    const std::regex reMapData{R"(t:.*)"};
    Map parseMap(const std::smatch& match, ParseLoc& loc) {
      Map m;
      if (loc.it != loc.end && std::regex_match(*loc.it, reMapData))
        ++loc.it;
      while (loc.it != loc.end && !std::regex_match(*loc.it, reMap))
        m.map.push_back(*loc.it++);
      if (m.map.empty())
        m.map.push_back("ERROR: empty map");
      if (loc.it != loc.end && loc.it->front() == '-')
        ++loc.it;
      return m;
    }
    std::string toString(const Map& m) {
      return "t:" + std::to_string(m.time) + " g:" + std::to_string(m.gas) + '\n' +
          util::joinLines(m.map);
    }

    const std::regex reRun{R"(run\s*.*)"};
    const std::regex reRunUntilIdle{R"(run\s*until\s+idle\s+(\w+))"};
    Item parseRun(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, reRunUntilIdle))
        return ++loc.it, Run{match[1]};
      return item::Error{*loc.it++ + " <--- cannot parse"};
    }
    std::string toString(const Run& r) { return "run until idle " + r.untilIdle; }

    const std::regex reCmdSelect{R"(select\s+(\w+(?:,\w+)*))"};
    const std::regex reCmdTrigger{R"(trigger\s+(\w+)\s+(\d+),(\d+))"};
    Item parseCommand(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, reCmdSelect))
        return ++loc.it, Command{cmd::Select{commaSplit(match[1])}};
      if (std::regex_match(*loc.it, match, reCmdTrigger)) {
        rts::Point target{std::stoi(match[2]), std::stoi(match[3])};
        return ++loc.it, Command{cmd::Trigger{match[1], target}};
      }
      return item::Error{*loc.it++ + " <--- cannot parse command"};
    }
    std::string toString(const Command& command) {
      return std::visit([](const auto& c) { return cmd::toString(c); }, command);
    }
  }
}

namespace test::seq {
  namespace {
    Item parse(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, item::reComment))
        return ++loc.it, item::parseComment(match);
      if (std::regex_match(*loc.it, match, item::reDefinition))
        return ++loc.it, item::parseDefinition(match);
      if (std::regex_match(*loc.it, match, item::reAssignment))
        return ++loc.it, item::parseAssignment(match);
      if (std::regex_match(*loc.it, match, item::reMap))
        return ++loc.it, item::parseMap(match, loc);
      if (std::regex_match(*loc.it, match, item::reRun))
        return item::parseRun(loc);
      return item::parseCommand(loc);
    }

    std::string separator(const Item* item1, const Item* item2) {
      auto* map1{std::get_if<item::Map>(item1)};
      auto* map2{std::get_if<item::Map>(item2)};
      auto* someMap{map1 ? map1 : map2 ? map2 : nullptr};

      std::string dashes;
      if (someMap) {
        auto width{someMap->map.front().size()};
        char c{map1 && map2 ? '=' : '-'};
        dashes += (!item1 || map1) ? "" : "\n";
        dashes += std::string(width, c);
      }

      return dashes + ((item1 || map2) ? "\n" : "");
    }
  }
}

auto test::seq::parse(const LineVector& input) -> Sequence {
  Sequence s;
  ParseLoc loc{input.begin(), input.end()};
  while (loc.it != loc.end)
    s.push_back(parse(loc));
  return s;
}

std::string test::seq::toString(const Sequence& sequence) {
  std::string r;
  const Item* prev{};
  for (const Item& item : sequence) {
    r += separator(prev, &item);
    r += toString(item);
    prev = &item;
  }
  r += separator(prev, nullptr);
  return r;
}

std::string test::seq::toString(const Item& item) {
  return std::visit([](const auto& a) { return item::toString(a); }, item);
}
