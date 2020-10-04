#include <igclib/time.hpp>
#include <math.h>
#include <stdexcept>
#include <string>

// default constructor, represents midnight
Time::Time() : m_hour(0), m_minute(0), m_second(0), m_sec_from_midnight(0) {}

// seconds is the number of seconds since midnight
Time::Time(int seconds) {
  this->check(seconds);
  this->m_sec_from_midnight = seconds;
  this->m_hour = std::floor((double)seconds / 3600.0);
  this->m_minute = std::floor(((double)seconds - 3600.0 * this->m_hour) / 60.0);
  this->m_second = seconds % 60;
}

Time::Time(int hour, int minute, int second)
    : m_hour(hour), m_minute(minute), m_second(second) {
  this->check(hour, minute, second);
  this->m_sec_from_midnight =
      3600 * this->m_hour + 60 * this->m_minute + this->m_second;
}

Time::Time(int hour, int minute, int second, const Time &offset)
    : Time(hour, minute, second) {
  this->operator+=(offset);
}

Time::Time(const Time &other) : Time(other.m_sec_from_midnight) {}

bool Time::zero() const { return this->m_sec_from_midnight == 0; }

Time &Time::operator++() {
  this->operator+=(Time(0, 0, 1));
  return *this;
}

Time &Time::operator--() {
  Time second(0, 0, 1);
  this->operator-=(second);
  return *this;
}

Time &Time::operator+=(const Time &other) {
  *this = this->operator+(other);
  return *this;
}

Time &Time::operator-=(const Time &other) {
  *this = this->operator-(other);
  return *this;
}

Time &Time::operator=(const Time &other) {
  this->m_hour = other.m_hour;
  this->m_minute = other.m_minute;
  this->m_second = other.m_second;
  this->m_sec_from_midnight = other.m_sec_from_midnight;
  return *this;
}

Time Time::operator+(const Time &other) const {
  return Time(this->m_sec_from_midnight + other.m_sec_from_midnight);
}

Time Time::operator-(const Time &other) const {
  return Time(this->m_sec_from_midnight - other.m_sec_from_midnight);
}

bool Time::operator==(const Time &other) const {
  return this->m_sec_from_midnight == other.m_sec_from_midnight;
}

bool Time::operator!=(const Time &other) const {
  return this->m_sec_from_midnight != other.m_sec_from_midnight;
}

bool Time::operator<(const Time &other) const {
  return this->m_sec_from_midnight < other.m_sec_from_midnight;
}

bool Time::operator<=(const Time &other) const {
  return this->m_sec_from_midnight <= other.m_sec_from_midnight;
}

bool Time::operator>(const Time &other) const {
  return this->m_sec_from_midnight > other.m_sec_from_midnight;
}

bool Time::operator>=(const Time &other) const {
  return this->m_sec_from_midnight >= other.m_sec_from_midnight;
}

std::string Time::to_string() const {
  char timechar[9];
  snprintf(timechar, 9, "%02d:%02d:%02d", this->m_hour, this->m_minute,
           this->m_second);
  return timechar;
}

/**
 * @brief Returns the number of seconds since midnight (00:00:00)
 */
int Time::to_seconds() const { return m_sec_from_midnight; }

void Time::check(int hour, int minute, int second) const {
  if ((hour < 0) || (hour > 23)) {
    throw std::runtime_error("hour must be between 0 and 23");
  }
  if ((minute < 0) || (minute > 59)) {
    throw std::runtime_error("minute must be between 0 and 59");
  }
  if ((second < 0) || (second > 59)) {
    throw std::runtime_error("second must be between 0 and 59");
  }
}

void Time::check(int seconds) const {
  if (seconds < 0) {
    throw std::runtime_error("time underflow");
  }
  if (seconds >= 86400) {
    throw std::runtime_error("time overflow");
  }
}
