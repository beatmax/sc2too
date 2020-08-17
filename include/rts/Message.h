#pragma once

#include "constants.h"
#include "types.h"

#include <array>

namespace rts {
  struct Message {
    GameTime time;
    char text[MaxMessageSize + 1];
  };

  class MessageList {
  public:
    void add(const World& w, const char* text);
    size_t size() const { return size_; }
    const Message& operator[](size_t i) const;

  private:
    std::array<Message, MaxMessages> messages_;
    size_t last_{0};
    size_t size_{0};
  };
}
