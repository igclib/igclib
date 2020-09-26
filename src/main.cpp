#include <igclib/airspace.hpp>
#include <igclib/config.hpp>
#include <igclib/flight.hpp>
#include <igclib/logging.hpp>
#include <igclib/race.hpp>
#include <igclib/task.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

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
  } catch (const std::runtime_error &e) {
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

void command_race(const std::string &flight_dir, const std::string &task_file,
                  const std::string &output) {
  if (task_file.empty()) {
    logging::error({"No task file provided"});
    exit(EXIT_FAILURE);
  }
  if (flight_dir.empty()) {
    logging::error({"No flight directory provided"});
    exit(EXIT_FAILURE);
  }
  try {
    Race race(flight_dir, task_file);
    race.save(output);
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

  // TODO getopt (list of names, check i+1, not other opt)
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
    } else if ((arg == "--pretty") || (arg == "-p")) {
      logging::set_pretty_print(true);
    }
  }

  // parse command
  arg = argv[1];
  if (arg == "version") {
    print_version();
  } else if (arg == "flightinfo") {
    command_xc(flight_file, airspace_file, output, force_xc);
  } else if (arg == "taskinfo") {
    command_opti(task_file, output);
  } else if (arg == "raceinfo") {
    command_race(flight_file, task_file, output);
  } else {
    logging::error({"Unkown command", argv[1]});
    usage();
  }

  return EXIT_SUCCESS;
}