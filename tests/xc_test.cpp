#include "igclib/flight.hpp"
#include "assert.h"

int main(int argc, char *argv[]){
  Flight f("flights/xc_col_agnel.igc");
  assert (f.pilot_name == "Téo Bouvard");

  return EXIT_SUCCESS;
}