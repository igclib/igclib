#include <igclib/time.hpp>
#include <string>

IGCTime::IGCTime(const std::string &str) {
  this->m_hour = std::stoi(str.substr(1, 2));
  this->m_minute = std::stoi(str.substr(3, 2));
  this->m_second = std::stoi(str.substr(5, 2));
  this->check(this->m_hour, this->m_minute, this->m_second);
  this->m_sec_from_midnight =
      3600 * this->m_hour + 60 * this->m_minute + this->m_second;
}