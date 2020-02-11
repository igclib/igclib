#pragma once

#include <string>
#include <map>
#include <functional>

class Flight {

public:
  Flight(const std::string filename);
  void to_JSON();

private:
  void process_H_record(const std::string& record);
  std::string pilot_name;
};