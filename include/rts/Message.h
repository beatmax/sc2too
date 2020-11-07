#pragma once

#include "constants.h"
#include "types.h"
#include "util/CircularBuffer.h"

namespace rts {
  struct Message {
    GameTime time;
    char text[MaxMessageSize + 1];
  };

  class MessageList {
  public:
    void add(const World& w, const char* text);
    void clear() { messages_.clear(); }
    size_t size() const { return messages_.size(); }
    const Message& operator[](size_t i) const { return messages_[i]; }

  private:
    util::CircularBuffer<Message, MaxMessages> messages_;
  };
}
