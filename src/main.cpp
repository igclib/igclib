#include "igclib/airspace.hpp"
#include "igclib/flight.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <iostream>

void usage() { std::cerr << "igclib command-line tool" << std::endl; }

void command_xc(const std::string &flight_file,
                const std::string &airspace_file) {
  if (flight_file.empty()) {
    std::cerr << "No flight file provided" << std::endl;
    exit(EXIT_FAILURE);
  }

  try {
    Flight flight(flight_file);
    if (!airspace_file.empty()) {
      Airspace airspace(airspace_file);
      flight.validate(airspace);
    }
    flight.to_JSON();
  } catch (std::runtime_error &e) {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    exit(EXIT_SUCCESS);
  }

  std::string flight_file;
  std::string airspace_file;
  std::string command;

  for (int i = 2; i < argc - 1; i++) {
    if (!strcmp(argv[i], "--flight")) {
      flight_file = argv[++i];
    } else if (!strcmp(argv[i], "--airspace")) {
      airspace_file = argv[++i];
    }
  }

  if (!strcmp(argv[1], "xc")) {
    command_xc(flight_file, airspace_file);
  }

  return EXIT_SUCCESS;
}