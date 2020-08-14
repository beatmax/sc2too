#include "util/debug.h"

#include <fstream>

std::ostream& util::trace() {
  static std::ofstream f{"/tmp/sc2too.trace"};
  return f;
}
