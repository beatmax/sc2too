#include "rts/Message.h"

#include "rts/World.h"

#include <cassert>
#include <cstring>

void rts::MessageList::add(const World& w, const char* text) {
  if (size_ < MaxMessages)
    ++size_;
  last_ = (last_ ? last_ : MaxMessages) - 1;
  auto& msg{messages_[last_]};
  msg.time = w.time;
  strncpy(msg.text, text, MaxMessageSize);
}

const rts::Message& rts::MessageList::operator[](size_t i) const {
  assert(i < size_);
  return messages_[(last_ + i) % MaxMessages];
}
