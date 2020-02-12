#include "igclib/airspace.hpp"
#include <fstream>
#include <string>

Airspace::Airspace(const std::string &airspace_file) {
  std::ifstream f;
  f.open(airspace_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + airspace_file + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  while (std::getline(f, line)) {
  }

  f.close();
}