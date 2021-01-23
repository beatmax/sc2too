#include "ui/IO.h"

#include "IOState.h"
#include "X.h"
#include "graph.h"
#include "ui/dim.h"

#include <chrono>
#include <clocale>
#include <codecvt>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <langinfo.h>
#include <locale>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

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

void ui::IO::supervisor() {
  X::saveState();
  pid_t pid{fork()};
  if (pid < 0) {
    std::perror("fork");
    std::_Exit(EXIT_FAILURE);
  }
  if (pid == 0)
    return;

  int status;
  if (wait(&status) < 0) {
    std::perror("wait");
    std::_Exit(EXIT_FAILURE);
  }
  if (!WIFEXITED(status)) {
    X::restoreState();
    std::cerr << "Game crashed :-(" << std::endl;
  }
  std::exit(EXIT_SUCCESS);
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

void ui::IO::onGameStart(const rts::World& w) {
  output.onGameStart(w);
}

bool ui::IO::paused() const {
  return state_->paused();
}

bool ui::IO::quit() const {
  return state_->quit;
}

ui::ScreenVector ui::IO::minimapSize() const {
  return {dim::MinimapArea.size.x, dim::MinimapArea.size.y * 2};
}
