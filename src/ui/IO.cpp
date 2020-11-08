#include "ui/IO.h"

#include "IOState.h"
#include "X.h"
#include "graph.h"

#include <chrono>
#include <clocale>
#include <codecvt>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <langinfo.h>
#include <locale>
#include <string>
#include <thread>

namespace {
  ui::IO* gIO{};

  void abrtHandler(int) {
    using namespace std::chrono_literals;
    std::cout << "Terminating in 10 seconds..." << std::endl;
    std::this_thread::sleep_for(10s);
    gIO->~IO();
    std::_Exit(EXIT_FAILURE);
  }
}

ui::IO::IO() : state_{std::make_unique<IOState>()}, input{*state_}, output{*state_} {
  gIO = this;
  std::signal(SIGABRT, abrtHandler);
  std::setlocale(LC_ALL, "");
  std::locale utf8loc{""};
  std::locale::global(std::locale{utf8loc, new std::codecvt_utf8<wchar_t>});
  if (std::string(nl_langinfo(CODESET)) != "UTF-8")
    throw std::runtime_error{"please select a UTF-8 locale"};
  X::init();
  graph::init();
  output.init();
  input.init();
}

ui::IO::~IO() {
  input.finish();
  X::finish();
}

bool ui::IO::paused() const {
  return state_->menu.active();
}

bool ui::IO::quit() const {
  return state_->quit;
}
