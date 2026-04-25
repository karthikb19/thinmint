#include <iostream>

#include "thinmint/core/version.h"

int main() {
  std::cout << "thinmint " << thinmint::core::version() << '\n';
  return 0;
}
