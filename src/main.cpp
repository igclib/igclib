#include <iostream>
#include "igclib/flight.hpp"
#include "stdio.h"
#include "string.h"

void usage(){
  std::cerr << "igclib command-line tool" << std::endl;
}

void command_xc(std::string flight_file, std::string airspace_file){
  if (flight_file.empty()){
    std::cerr << "No flight file provided" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (!airspace_file.empty()){
    std::cerr << airspace_file << std::endl;
  }

  try {
    Flight f(flight_file);
    f.json();
  } catch (std::runtime_error& e){
    std::cout << e.what() << std::endl;
  }
}


int main(int argc, char *argv[]){
  if (argc < 2){
    usage();
    exit(EXIT_SUCCESS);
  }

  std::string flight_file;
  std::string airspace_file;
  std::string command;

  for (int i = 2; i < argc-1; i++){
    if (!strcmp(argv[i], "--flight")){
      flight_file = argv[++i];
    }
  }

  if (!strcmp(argv[1], "xc")){
    command_xc(flight_file, airspace_file);
  }

  return EXIT_SUCCESS;
}