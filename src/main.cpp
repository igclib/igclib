#include <igclib/airspace.hpp>
#include <igclib/config.hpp>
#include <igclib/flight.hpp>
#include <igclib/logging.hpp>
#include <igclib/task.hpp>
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
    logging::error({"No flight file provided"});
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
    exit(EXIT_SUCCESS);
  } catch (std::runtime_error &e) {
    logging::error({e.what()});
    exit(EXIT_FAILURE);
  }
}

void command_opti(const std::string &task_file, const std::string &output) {
  if (task_file.empty()) {
    logging::error({"No task file provided"});
    exit(EXIT_FAILURE);
  }
  try {
    Task task(task_file);
    task.save(output);
  } catch (std::runtime_error &e) {
    logging::error({e.what()});
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage();
    exit(EXIT_SUCCESS);
  }

  std::string arg;
  std::string flight_file;
  std::string airspace_file;
  std::string task_file;
  std::string command;
  std::string output;
  bool force_xc = false; // TODO remove once xc optimization works

  for (int i = 2; i < argc; i++) {
    arg = argv[i];

    if ((arg == "--flight") && (i + 1 < argc)) {
      flight_file = argv[++i];
    } else if (((arg == "--airspace") || ((arg == "-a"))) && (i + 1 < argc)) {
      airspace_file = argv[++i];
    } else if (((arg == "--output") || (arg == "-o")) && (i + 1 < argc)) {
      output = argv[++i];
    } else if ((arg == "--task") && (i + 1 < argc)) {
      task_file = argv[++i];
    } else if (arg == "--force_xc") {
      force_xc = true;
    } else if ((arg == "--quiet") || (arg == "-q")) {
      logging::set_level(logging::verbosity::QUIET);
    } else if ((arg == "--verbose") || (arg == "-v")) {
      logging::set_level(logging::verbosity::DEBUG);
    }
  }

  // parse command
  arg = argv[1];
  if (arg == "version") {
    print_version();
  } else if (arg == "xc") {
    command_xc(flight_file, airspace_file, output, force_xc);
  } else if (arg == "opti") {
    command_opti(task_file, output);
  } else {
    logging::error({"Unkown command", argv[1]});
    usage();
  }

  return EXIT_SUCCESS;
}