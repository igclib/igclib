#include "igclib/flight.hpp"
#include "assert.h"
#include <iostream>

int main(){
  Flight f("data/flights/xc_col_agnel.igc");
  if (f.pilot_name != "Téo Bouvard"){
    std::cerr << "Test failed" << std::endl;
  }

  std::cerr << "Test passed" << std::endl;
  return EXIT_SUCCESS;
}