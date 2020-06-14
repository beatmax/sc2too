#include "ui/IO.h"

#include "IOState.h"
#include "X.h"

#include <clocale>
#include <codecvt>
#include <locale>

ui::IO::IO() : state_{std::make_unique<IOState>()}, input{*state_}, output{*state_} {
  setlocale(LC_ALL, "en_US.UTF-8");
  std::locale utf8loc{"en_US.UTF-8"};
  std::locale::global(std::locale{utf8loc, new std::codecvt_utf8<wchar_t>});
  X::init();
  output.init();
  input.init();
}

ui::IO::~IO() {
  X::finish();
}

bool ui::IO::paused() const {
  return state_->menu.active();
}

bool ui::IO::quit() const {
  return state_->quit;
}
