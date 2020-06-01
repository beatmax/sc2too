#include "util/fs.h"

#include <istream>

template<typename CharT>
std::vector<std::basic_string<CharT>> util::fs::readLines(std::basic_istream<CharT>& is) {
  std::vector<std::basic_string<CharT>> lines;
  std::basic_string<CharT> line;
  while (std::getline(is, line))
    lines.push_back(std::move(line));
  return lines;
}

template std::vector<std::string> util::fs::readLines(std::istream& is);
template std::vector<std::wstring> util::fs::readLines(std::wistream& is);
