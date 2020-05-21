#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace util::fs {
  std::vector<std::string> readLines(std::istream& is);
}
