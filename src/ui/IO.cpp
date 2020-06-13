#include "ui/IO.h"

#include "X.h"

#include <clocale>
#include <codecvt>
#include <locale>

ui::IO::IO() {
  setlocale(LC_ALL, "en_US.UTF-8");
  std::locale utf8loc{"en_US.UTF-8"};
  std::locale::global(std::locale{utf8loc, new std::codecvt_utf8<wchar_t>});
  X::init();
  input.init(menu);
  output.init(menu);
}

ui::IO::~IO() {
  X::finish();
}
