#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "igclib/airspace.hpp"
int main() {

  std::vector<std::string> failures;

  Airspace airspace("data/airspace/reno.txt");

  // Number of airspaces in file (24 - 2 unsupported)
  if (airspace.size() != 22) {
    failures.push_back("Wrong number of zones in Airspace. Expected 24, got " +
                       airspace.size());
  }

  if (!failures.empty()) {
    for (std::string s : failures) {
      std::cerr << s << std::endl;
    }
  } else {
    std::cerr << "Test passed" << std::endl;
  }
  return EXIT_SUCCESS;
}