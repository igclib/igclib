#include <igclib/airspace.hpp>
#include <igclib/logging.hpp>
#include <iostream>
#include <string>
#include <vector>

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
    logging::info({"[airspace counting] test passed"});
  }
  return EXIT_SUCCESS;
}