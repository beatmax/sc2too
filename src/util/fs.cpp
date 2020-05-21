#include "util/fs.h"

#include <istream>

std::vector<std::string> util::fs::readLines(std::istream& is) {
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(is, line))
    lines.push_back(std::move(line));
  return lines;
}
