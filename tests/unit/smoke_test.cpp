#include <cstdlib>
#include <iostream>
#include <string>

#include "thinmint/core/version.h"

int main() {
  const std::string version(thinmint::core::version());
  if (version.empty()) {
    std::cerr << "version string must not be empty\n";
    return EXIT_FAILURE;
  }

  if (version.find("thinmint") == std::string::npos) {
    std::cerr << "version string should identify thinmint\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
