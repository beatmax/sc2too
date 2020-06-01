#include "ui/IO.h"

#include <clocale>
#include <codecvt>
#include <locale>

ui::IO::IO() {
  setlocale(LC_ALL, "en_US.UTF-8");
  std::locale utf8loc{"en_US.UTF-8"};
  std::locale::global(std::locale{utf8loc, new std::codecvt_utf8<wchar_t>});
  output.init();
}
