#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/time.hpp>

class Turnpoint {
public:
  Turnpoint(){};
  Turnpoint(const GeoPoint &center, const std::size_t &radius,
            const std::string &name, const std::string &desc);
  virtual std::string to_string() const;

protected:
  GeoPoint m_center;
  std::size_t m_radius;
  std::string m_name;
  std::string m_desc;
};

class Takeoff : public Turnpoint {
public:
  Takeoff(){};
  Takeoff(const GeoPoint &center, const std::size_t &radius,
          const std::string &name, const std::string &desc);

  std::string to_string() const;
};

class SSS : public Turnpoint {
public:
  SSS(){};
  SSS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc, const Time &open);

  std::string to_string() const;

protected:
  Time m_open;
};

class ESS : public Turnpoint {
public:
  ESS(){};
  ESS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc);

  std::string to_string() const;

protected:
};

class Goal : public Turnpoint {
public:
  Goal(){};
  Goal(const GeoPoint &center, const std::size_t &radius,
       const std::string &name, const std::string &desc, const Time &close,
       bool line);

  std::string to_string() const;
  bool is_line() const;

protected:
  Time m_close;
  bool m_line;
};
