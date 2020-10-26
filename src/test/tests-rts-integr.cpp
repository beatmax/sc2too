#include "catch2/catch.hpp"
#include "tests-rts-seq-run.h"
#include "util/algorithm.h"
#include "util/fs.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
namespace seq = test::seq;

namespace {
  constexpr auto* OutputFile{".test.out"};

  void runTest(const fs::path& filepath) {
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
      std::system((std::string{"vimdiff "} + filepath.string() + " " + OutputFile +
                   " < /dev/tty >& /dev/tty")
                      .c_str());
      FAIL("Integration test failed: " + filepath.string());
    }
  }
}

TEST_CASE("Integration", "[rts]") {
  std::remove(OutputFile);
  for (const auto& p : fs::recursive_directory_iterator("test")) {
    if (fs::is_regular_file(p))
      runTest(p);
  }
}
