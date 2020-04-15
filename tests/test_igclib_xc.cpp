#include <assert.h>
#include <igclib/flight.hpp>
#include <igclib/json.hpp>
#include <iostream>

int main() {
  Flight f("data/flights/xc_col_agnel.igc");
  json j = f.to_json();
  if (j["pilot"] != "Téo Bouvard") {
    std::cerr << "Test failed" << std::endl;
  }

  std::cerr << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}