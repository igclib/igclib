#include <igclib/time.hpp>
#include <stdexcept>
#include <string>

XCTaskTime::XCTaskTime(const std::string &str) {
  try {
    this->m_hour = std::stoi(str.substr(0, 2));
    this->m_minute = std::stoi(str.substr(3, 2));
    this->m_second = std::stoi(str.substr(7, 2));
  } catch (const std::invalid_argument &err) {
    throw std::runtime_error("could not parse time");
  }
  this->check(this->m_hour, this->m_minute, this->m_second);
  this->m_sec_from_midnight =
      3600 * this->m_hour + 60 * this->m_minute + this->m_second;
}