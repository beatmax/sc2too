#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace util {
  class ProgramOptions {
  public:
    ProgramOptions(int argc, char** argv) {
      for (int i{1}; i < argc; ++i)
        args.emplace_back(argv[i]);
    }

    bool operator[](const std::string& opt) const {
      return std::find(args.begin(), args.end(), opt) != args.end();
    }

    std::string arg(const std::string& opt, const std::string& def = "") const {
      auto it{std::find(args.begin(), args.end(), opt)};
      if (it != args.end() && ++it != args.end())
        return *it;
      return def;
    }

  private:
    std::vector<std::string> args;
  };
}
