#include <igclib/flight.hpp>
#include <igclib/json.hpp>
#include <igclib/logging.hpp>
#include <iostream>

int main() {
  Flight f("data/xc/col_agnel.igc");

  // pilot name
  json j = f.to_json();
  if (j["pilot"] != "Téo Bouvard") {
    logging::error({"[pilot name] test failed"});
  }

  logging::info({"test passed"});
  return EXIT_SUCCESS;
}