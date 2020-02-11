#include "igclib/flight.hpp"
#include <fstream>
#include <iostream>
#include <string>

Flight::Flight(std::string filename) {
  std::ifstream igc;
  igc.open(filename);
  if (!igc.is_open()) {
    std::string error = "Could not open file '" + filename + "'";
    throw std::runtime_error(error);
  }
  std::cerr << "Opened file " << filename << std::endl;
  igc.close();
}

void Flight::json() {
  std::cerr << "json serialization of flight" << std::endl;
}