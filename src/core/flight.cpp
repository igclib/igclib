#include "igclib/flight.hpp"
#include <fstream>
#include <iostream>
#include <string>

Flight::Flight(std::string filename) {
  std::ifstream f;
  f.open(filename);
  if (!f.is_open()) {
    std::string error = "Could not open file '" + filename + "'";
    throw std::runtime_error(error);
  }

  std::string line;
  while (std::getline(f, line)) {
    switch (line[0]){
    case 'H':
      process_H_record(line);
      break;
    default:
      break;
    }
  }

  f.close();
}

void Flight::process_H_record(const std::string& record){
  std::cout << record << std::endl;
}

void Flight::to_JSON() {
  std::cerr << "JSON serialization of flight" << std::endl;
}