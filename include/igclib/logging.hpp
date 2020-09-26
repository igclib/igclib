#pragma once

#include <iostream>
#include <string>

namespace logging {
typedef enum verbosity { QUIET, ERROR, WARNING, DEBUG } verbosity;
extern verbosity default_level;
extern bool pretty_print;

void set_level(verbosity level);
void set_pretty_print(bool enabled);
void error(const std::initializer_list<std::string> &msgs);
void warning(const std::initializer_list<std::string> &msgs);
void debug(const std::initializer_list<std::string> &msgs);
void info(const std::initializer_list<std::string> &msgs);
} // namespace logging
