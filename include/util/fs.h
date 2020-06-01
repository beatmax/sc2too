#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace util::fs {
  template<typename CharT>
  std::vector<std::basic_string<CharT>> readLines(std::basic_istream<CharT>& is);
}
