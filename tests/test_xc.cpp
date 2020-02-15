#include "assert.h"
#include "igclib/flight.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main() {
  Flight f("data/flights/xc_col_agnel.igc");
  json j = f.serialize();
  if (j["pilot"] != "Téo Bouvard") {
    std::cerr << "Test failed" << std::endl;
  }

  std::cerr << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}