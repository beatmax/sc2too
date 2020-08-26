#include "ui/IO.h"

#include "IOState.h"
#include "X.h"

#include <chrono>
#include <clocale>
#include <codecvt>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <locale>
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
  std::setlocale(LC_ALL, "en_US.UTF-8");
  std::locale utf8loc{"en_US.UTF-8"};
  std::locale::global(std::locale{utf8loc, new std::codecvt_utf8<wchar_t>});
  X::init();
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
