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

namespace test::seq::item::act {
  namespace {
    std::string toString(const Add& a) { return "add " + a.name + ' ' + ::toString(a.point); }
    std::string toString(const Destroy& d) { return "destroy " + util::join(d.names, ','); }
    std::string toString(const Provision& p) {
      return "provision " + p.resource + ' ' + std::to_string(p.quantity);
    }
    std::string toString(const Allocate& a) {
      return "allocate " + a.resource + ' ' + std::to_string(a.quantity);
    }
  }
}

namespace test::seq::item::cmd {
  namespace {
    std::string toString(const BuildPrototype& bp) { return "build_prototype " + bp.type; }
    std::string toString(const Select& s) { return "select " + util::join(s.names, ','); }
    std::string toString(const Trigger& t) {
      return (t.enqueue ? "enqueue " : "trigger ") + t.ability +
          (t.target ? (' ' + ::toString(*t.target)) : "");
    }
  }
}

namespace test::seq::item {
  namespace {
    std::string toString(const Error& e) { return e.text; }

    const std::regex reEmpty{R"(^\s*$)"};
    std::string toString(const Empty& e) { return ""; }

    const std::regex reComment{R"(#(.*))"};
    Comment parseComment(const std::smatch& match) { return {match[1]}; }
    std::string toString(const Comment& c) { return '#' + c.text; }

    const std::regex reOption{R"(option\s+(\w+(?:,\w+)*))"};
    Option parseOption(const std::smatch& match) { return {commaSplit(match[1])}; }
    std::string toString(const Option& o) { return "option " + util::join(o.options, ','); }

    const std::regex reMessage{R"(message: (.*))"};
    Message parseMessage(const std::smatch& match) { return {match[1]}; }
    std::string toString(const Message& m) { return "message: " + m.text; }

    const std::regex rePrototype{R"(prototype: (.*))"};
    Prototype parsePrototype(const std::smatch& match) { return {match[1]}; }
    std::string toString(const Prototype& p) { return "prototype: " + p.type; }

    const std::regex reDefinition{R"((\w+(?:,\w+)*)\s*:\s*(\w+))"};
    Definition parseDefinition(const std::smatch& match) {
      return {commaSplit(match[1]), match[2]};
    }
    std::string toString(const Definition& d) { return util::join(d.names, ',') + ": " + d.type; }

    const std::regex reAssignment{R"((\w+)\((\d+),(\d+)\)\s*=\s*(\d+))"};
    Assignment parseAssignment(const std::smatch& match) {
      rts::Point target{std::stoi(match[2]), std::stoi(match[3])};
      return {target, match[1], std::stoi(match[4])};
    }
    std::string toString(const Assignment& a) {
      return a.property + '(' + ::toString(a.target) + ") = " + std::to_string(a.value);
    }

    const std::regex reMap{R"((?:-+|=+)(?:  .*)?)"};
    Map parseMap(const std::smatch& match, ParseLoc& loc) {
      Map m;
      while (loc.it != loc.end && !std::regex_match(*loc.it, reMap))
        m.map.push_back(*loc.it++);
      if (m.map.empty())
        m.map.push_back("ERROR: empty map");
      if (loc.it != loc.end && loc.it->front() == '-')
        ++loc.it;
      return m;
    }
    std::vector<std::string> mapInfo(const Map& m) {
      std::vector<std::string> info;
      if (m.time)
        info.push_back("t:" + std::to_string(*m.time));
      if (m.mineral)
        info.push_back("m:" + std::to_string(*m.mineral));
      if (m.gas)
        info.push_back("g:" + std::to_string(*m.gas));
      if (m.supply)
        info.push_back(
            "s:" + std::to_string(m.supply->first) + '/' + std::to_string(m.supply->second));
      return info;
    }
    std::string toString(const Map& m) { return util::joinLines(m.map); }

    const std::regex reRun{R"(run(?:\s+.*)?)"};
    const std::regex reRunDefault{R"(run\s*)"};
    const std::regex reRunFor{R"(run\s+for\s+(\d+))"};
    const std::regex reRunUntilIdle{R"(run\s+until\s+idle\s+(\w+))"};
    Item parseRun(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, reRunDefault))
        return ++loc.it, Run{};
      if (std::regex_match(*loc.it, match, reRunFor))
        return ++loc.it, Run{std::stoi(match[1])};
      if (std::regex_match(*loc.it, match, reRunUntilIdle))
        return ++loc.it, Run{std::nullopt, match[1]};
      return item::Error{*loc.it++ + " <--- cannot parse"};
    }
    std::string toString(const Run& r) {
      return r.duration ? "run for " + std::to_string(*r.duration)
          : r.untilIdle ? "run until idle " + *r.untilIdle
                        : "run";
    }

    const std::regex reAction{R"((?:add|destroy|provision|allocate)\s.*)"};
    const std::regex reActionAdd{R"(add\s+(\w+)\s+(\d+),(\d+))"};
    const std::regex reActionDestroy{R"(destroy\s+(\w+(?:,\w+)*))"};
    const std::regex reActionProvision{R"(provision\s+(\w+)\s+(-?\d+))"};
    const std::regex reActionAllocate{R"(allocate\s+(\w+)\s+(-?\d+))"};
    Item parseAction(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, reActionAdd)) {
        rts::Point point{std::stoi(match[2]), std::stoi(match[3])};
        return ++loc.it, Action{act::Add{match[1], point}};
      }
      if (std::regex_match(*loc.it, match, reActionDestroy))
        return ++loc.it, Action{act::Destroy{commaSplit(match[1])}};
      if (std::regex_match(*loc.it, match, reActionProvision))
        return ++loc.it, Action{act::Provision{match[1], std::stoi(match[2])}};
      if (std::regex_match(*loc.it, match, reActionAllocate))
        return ++loc.it, Action{act::Allocate{match[1], std::stoi(match[2])}};
      return item::Error{*loc.it++ + " <--- cannot parse action"};
    }
    std::string toString(const Action& action) {
      return std::visit([](const auto& a) { return act::toString(a); }, action);
    }

    const std::regex reCommand{R"((?:build_prototype|select|trigger|enqueue)\s.*)"};
    const std::regex reCommandBuildPrototype{R"(build_prototype\s+(\w+))"};
    const std::regex reCommandSelect{R"(select\s+(\w+(?:,\w+)*))"};
    const std::regex reCommandTrigger{R"((trigger|enqueue)\s+(\w+)(?:\s+(\d+),(\d+))?)"};
    Item parseCommand(ParseLoc& loc) {
      std::smatch match;
      if (std::regex_match(*loc.it, match, reCommandBuildPrototype))
        return ++loc.it, Command{cmd::BuildPrototype{match[1]}};
      if (std::regex_match(*loc.it, match, reCommandSelect))
        return ++loc.it, Command{cmd::Select{commaSplit(match[1])}};
      if (std::regex_match(*loc.it, match, reCommandTrigger)) {
        std::optional<rts::Point> target;
        bool enqueue{match[1] == "enqueue"};
        if (match[3].matched && match[4].matched)
          target = rts::Point{std::stoi(match[3]), std::stoi(match[4])};
        return ++loc.it, Command{cmd::Trigger{match[2], target, enqueue}};
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
      if (std::regex_match(*loc.it, match, item::reEmpty))
        return ++loc.it, item::Empty{};
      if (std::regex_match(*loc.it, match, item::reComment))
        return ++loc.it, item::parseComment(match);
      if (std::regex_match(*loc.it, match, item::reOption))
        return ++loc.it, item::parseOption(match);
      if (std::regex_match(*loc.it, match, item::reMessage))
        return ++loc.it, item::parseMessage(match);
      if (std::regex_match(*loc.it, match, item::rePrototype))
        return ++loc.it, item::parsePrototype(match);
      if (std::regex_match(*loc.it, match, item::reDefinition))
        return ++loc.it, item::parseDefinition(match);
      if (std::regex_match(*loc.it, match, item::reAssignment))
        return ++loc.it, item::parseAssignment(match);
      if (std::regex_match(*loc.it, match, item::reMap))
        return ++loc.it, item::parseMap(match, loc);
      if (std::regex_match(*loc.it, match, item::reRun))
        return item::parseRun(loc);
      if (std::regex_match(*loc.it, match, item::reAction))
        return item::parseAction(loc);
      if (std::regex_match(*loc.it, match, item::reCommand))
        return item::parseCommand(loc);
      return item::Error{*loc.it++ + " <--- cannot parse"};
    }

    std::string separator(const Item* item1, const Item* item2) {
      auto* map1{std::get_if<item::Map>(item1)};
      auto* map2{std::get_if<item::Map>(item2)};
      auto* someMap{map1 ? map1 : map2 ? map2 : nullptr};

      std::string mapSeparator;
      if (someMap) {
        auto width{someMap->map.front().size()};
        char c{map1 && map2 ? '=' : '-'};
        mapSeparator += (!item1 || map1) ? "" : "\n";
        mapSeparator += std::string(width, c);
        if (map2) {
          if (auto info{item::mapInfo(*map2)}; !info.empty())
            mapSeparator += "  [" + util::join(item::mapInfo(*map2), ' ') + "]";
        }
      }

      return mapSeparator + ((item1 || map2) ? "\n" : "");
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
