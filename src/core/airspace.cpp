#include "igclib/airspace.hpp"
#include "igclib/zone.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

Airspace::Airspace(const std::string &airspace_file) {
  std::ifstream f;
  f.open(airspace_file);
  if (!f.is_open()) {
    const std::string error = "Could not open file '" + airspace_file + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  std::vector<std::string> record;
  while (std::getline(f, line)) {
    if (line.substr(0, 2) == "AC") {
      if (!record.empty()) {
        this->zones.push_back(record);
        record.clear();
      }
    }
    record.push_back(line);
  }

  #ifndef NDEBUG
  std::cerr << this->zones.size() << " zones in airspace" << std::endl;
  #endif

  f.close();
}