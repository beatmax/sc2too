#pragma once

#include "rts/types.h"

#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace test::seq {
  using LineVector = std::vector<std::string>;

  namespace item {
    struct Error {
      std::string text;
    };

    struct Empty {};

    struct Comment {
      std::string text;
    };

    struct Option {
      std::vector<std::string> options;
    };

    struct Definition {
      std::vector<std::string> names;
      std::string type;
    };

    struct Assignment {
      rts::Point target;
      std::string property;
      int32_t value;
    };

    struct Map {
      LineVector map;
      std::optional<rts::GameTime> time;
      std::optional<rts::Quantity> gas;
      std::optional<std::pair<rts::Quantity, rts::Quantity>> supply;

      bool diff(const Map& other) const {
        return map != other.map || gas != other.gas || supply != other.supply;
      }
    };

    struct Message {
      std::string text;
    };

    struct Prototype {
      std::string type;
    };

    struct Run {
      std::optional<rts::GameTime> duration;
      std::optional<std::string> untilIdle;
    };

    namespace act {
      struct Add {
        std::string name;
        rts::Point point;
      };

      struct Destroy {
        std::vector<std::string> names;
      };

      struct Provision {
        std::string resource;
        rts::Quantity quantity;
      };

      struct Allocate {
        std::string resource;
        rts::Quantity quantity;
      };
    }

    using Action = std::variant<act::Add, act::Destroy, act::Provision, act::Allocate>;

    namespace cmd {
      struct BuildPrototype {
        std::string type;
      };

      struct Select {
        std::vector<std::string> names;
      };

      struct Trigger {
        std::string ability;
        std::optional<rts::Point> target;
        bool enqueue;
      };
    }

    using Command = std::variant<cmd::BuildPrototype, cmd::Select, cmd::Trigger>;
  }

  using Item = std::variant<
      item::Error,
      item::Empty,
      item::Comment,
      item::Option,
      item::Definition,
      item::Assignment,
      item::Map,
      item::Message,
      item::Prototype,
      item::Run,
      item::Action,
      item::Command>;
  using Sequence = std::vector<Item>;

  Sequence parse(const LineVector& input);
  std::string toString(const Sequence& sequence);
  std::string toString(const Item& item);
}
