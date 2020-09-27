#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

/**
 * @brief Dumps a JSON object to a file
 *
 * @param to_save json object to be dumped
 * @param out output file name ("-" for stdout)
 * @param indent if nonnegative, array elements and object
 * members will be pretty-printed with that indent level. An indent level of `0`
 * will only insert newlines. `-1` (the default) selects the most compact
 * representation
 * @param indent_char character to use for indentation if indent is greater than
 * 0, defaults to ' ' (space)
 */
static void save_json(const json &to_save, const std::string &out,
                      int indent = -1, char indent_char = ' ') {
  if (out == "-" || out.empty()) {
    std::cout << to_save.dump(indent, indent_char, false,
                              json::error_handler_t::replace)
              << std::endl;
  } else {
    std::ofstream f;
    f.open(out);
    if (!f.is_open()) {
      const std::string error = "could not open file '" + out + "'";
      throw std::runtime_error(error);
    }
    f << to_save.dump(indent, indent_char, false,
                      json::error_handler_t::replace);
  }
}