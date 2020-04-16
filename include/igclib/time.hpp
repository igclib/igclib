#pragma once

#include <string>

class Time {
public:
  Time();
  Time(int seconds);
  Time(int hour, int minute, int second);
  Time(int hour, int minute, int second, const Time &offset);
  Time(const Time &other);
  bool operator==(const Time &other) const;
  bool operator!=(const Time &other) const;
  bool operator<(const Time &other) const;
  bool operator>(const Time &other) const;
  bool zero() const;
  Time &operator++();
  Time &operator--();
  Time operator+(const Time &other);
  Time operator-(const Time &other);
  Time &operator+=(const Time &other);
  Time &operator-=(const Time &other);
  Time &operator=(const Time &other);

  std::string to_string() const;

protected:
  void check(int hour, int minute, int second) const;
  void check(int seconds) const;

  int m_hour;
  int m_minute;
  int m_second;
  int m_sec_from_midnight;
};

class IGCTime : public Time {
public:
  IGCTime(const std::string &str);
};

class XCTaskTime : public Time {
public:
  XCTaskTime(const std::string &str);
};