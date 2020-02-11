#pragma once

#include <string>

class Flight {

public:
  Flight(const std::string& filename);
  void to_JSON();

  std::string pilot_name = "Unknown pilot";
  
private:
  void process_H_record(const std::string& record);
  int time_zone_offset = 0;
};