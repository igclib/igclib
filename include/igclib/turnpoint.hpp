#pragma once

#include <igclib/geopoint.hpp>
#include <igclib/json.hpp>
#include <igclib/time.hpp>

class Turnpoint {
public:
  Turnpoint() = default;
  Turnpoint(const GeoPoint &center, const std::size_t &radius,
            const std::string &name, const std::string &desc);
  // virtual bool validate(const GeoPoint &pos);
  virtual bool contains(const GeoPoint &pos);

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
  Takeoff() = default;
  Takeoff(const GeoPoint &center, const std::size_t &radius,
          const std::string &name, const std::string &desc);

  // display
  std::string to_string() const;
  json to_json() const;
};

class SSS : public Turnpoint {
public:
  SSS() = default;
  SSS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc, const Time &open,
      bool exit);
  const Time open() const { return this->m_open; };
  bool is_exit() const { return this->m_exit; }
  bool is_enter() const { return !this->m_exit; }

  // display
  std::string to_string() const;
  json to_json() const;

protected:
  Time m_open;
  bool m_exit;
};

class ESS : public Turnpoint {
public:
  ESS() = default;
  ESS(const GeoPoint &center, const std::size_t &radius,
      const std::string &name, const std::string &desc);

  // display
  std::string to_string() const;
  json to_json() const;

protected:
};

class Goal : public Turnpoint {
public:
  Goal() = default;
  Goal(const GeoPoint &center, const std::size_t &radius,
       const std::string &name, const std::string &desc, const Time &close,
       bool line);
  bool contains(const GeoPoint &pos);

  // get
  bool is_line() const;
  const Time close() const;

  // display
  std::string to_string() const;
  json to_json() const;

protected:
  Time m_close;
  bool m_line;
};
