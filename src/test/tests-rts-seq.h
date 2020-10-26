#pragma once

#include "rts/types.h"

#include <string>
#include <variant>
#include <vector>

namespace test::seq {
  using LineVector = std::vector<std::string>;

  namespace item {
    struct Error {
      std::string text;
    };

    struct Comment {
      std::string text;
    };

    struct Definition {
      std::string name;
      std::string type;
    };

    struct Assignment {
      rts::Point target;
      std::string property;
      int32_t value;
    };

    struct Map {
      LineVector map;
      rts::GameTime time{};
      rts::Quantity gas{};
    };

    struct Run {
      std::string untilIdle;
    };

    namespace cmd {
      struct Select {
        std::vector<std::string> names;
      };

      struct Trigger {
        std::string ability;
        rts::Point target;
      };
    }

    using Command = std::variant<cmd::Select, cmd::Trigger>;
  }

  using Item = std::variant<
      item::Error,
      item::Comment,
      item::Definition,
      item::Assignment,
      item::Map,
      item::Run,
      item::Command>;
  using Sequence = std::vector<Item>;

  Sequence parse(const LineVector& input);
  std::string toString(const Sequence& sequence);
  std::string toString(const Item& item);
}
