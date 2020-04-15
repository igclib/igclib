#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/time.hpp>

class Turnpoint {
public:
  Turnpoint(){};
  Turnpoint(const GeoPoint &center, const std::size_t &radius,
            const std::string &name, const std::string &desc);

  // get
  const GeoPoint &center() const;
  const std::size_t &radius() const;

  // display
  virtual std::string to_string() const;
  virtual json to_json() const;

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

  // display
  std::string to_string() const;
  json to_json() const;
};

class SSS : public Turnpoint {
public:
  SSS(){};
  SSS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc, const Time &open);

  // display
  std::string to_string() const;
  json to_json() const;

protected:
  Time m_open;
};

class ESS : public Turnpoint {
public:
  ESS(){};
  ESS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc);

  // display
  std::string to_string() const;
  json to_json() const;

protected:
};

class Goal : public Turnpoint {
public:
  Goal(){};
  Goal(const GeoPoint &center, const std::size_t &radius,
       const std::string &name, const std::string &desc, const Time &close,
       bool line);

  // get
  bool is_line() const;

  // display
  std::string to_string() const;
  json to_json() const;

protected:
  Time m_close;
  bool m_line;
};
