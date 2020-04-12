#include <igclib/airspace.hpp>
#include <igclib/config.hpp>
#include <igclib/flight.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

void usage() { std::cerr << USAGE << std::endl; }

void print_version() { std::cerr << VERSION << std::endl; }

void command_xc(const std::string &flight_file,
                const std::string &airspace_file, const std::string &output,
                bool force_xc) {
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
    if (force_xc) {
      flight.compute_score();
    }
    flight.save(output);
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
  std::string output;
  bool force_xc = false; // TODO remove once xc optimization works

  for (int i = 2; i < argc - 1; i++) {
    if (!strcmp(argv[i], "--flight")) {
      flight_file = argv[++i];
    } else if (!strcmp(argv[i], "--airspace")) {
      airspace_file = argv[++i];
    } else if (!strcmp(argv[i], "--output")) {
      output = argv[++i];
    } else if (!strcmp(argv[i], "--force_xc")) {
      force_xc = true;
    }
  }

  // parse command
  if (!strcmp(argv[1], "xc")) {
    command_xc(flight_file, airspace_file, output, force_xc);
  } else if (!strcmp(argv[1], "version")) {
    print_version();
  } else {
    std::cerr << "Unkown command : " << argv[1] << std::endl;
  }

  return EXIT_SUCCESS;
}