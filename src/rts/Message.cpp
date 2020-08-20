#include "rts/Message.h"

#include "rts/World.h"

#include <cstring>

void rts::MessageList::add(const World& w, const char* text) {
  Message& msg{messages_.push()};
  msg.time = w.time;
  strncpy(msg.text, text, MaxMessageSize);
}
