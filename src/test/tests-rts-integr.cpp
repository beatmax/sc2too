#include "catch2/catch.hpp"
#include "tests-rts-seq-run.h"
#include "util/algorithm.h"
#include "util/fs.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

namespace fs = std::filesystem;
namespace seq = test::seq;

namespace {
  constexpr auto* OutputFile{".test.out"};

  void runTest(const fs::path& filepath, bool vimdiff) {
    seq::LineVector input;
    {
      auto f{std::ifstream(filepath)};
      REQUIRE(f);
      input = util::fs::readLines(f);
    }
    const auto output{seq::toString(seq::run(seq::parse(input)))};
    if (util::joinLines(input) != output) {
      {
        std::ofstream f{OutputFile};
        f.write(output.data(), output.size());
      }
      if (vimdiff) {
        std::system((std::string{"vimdiff "} + filepath.string() + " " + OutputFile +
                     " < /dev/tty >& /dev/tty")
                        .c_str());
      }
      FAIL("Integration test failed: " + filepath.string());
    }
  }
}

TEST_CASE("Integration", "[rts]") {
  std::remove(OutputFile);
  std::optional<fs::path> runOnly;
  if (char* v{std::getenv("RTS_TEST_RUN_ONLY")})
    runOnly = v;
  bool ran{false};
  for (const auto& p : fs::recursive_directory_iterator("test")) {
    if (fs::is_regular_file(p)) {
      if (!runOnly || *runOnly == p) {
        runTest(p, !runOnly);
        ran = true;
      }
    }
  }
  if (!ran && runOnly) {
    std::cerr << "Invalid test: " << *runOnly << std::endl;
    exit(2);
  }
}
