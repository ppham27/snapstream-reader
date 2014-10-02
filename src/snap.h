#ifndef SNAP_H
#define SNAP_H

#include <algorithm>
#include <cmath>
#include <cctype>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "Program.h"

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(const std::string &pattern1,
                                               const std::string &pattern2,
                                               int distance,
                                               const std::string &s);
  std::vector<snap::Program> parse_programs(const std::string &file_name);
  std::vector<snap::Program> parse_programs(std::istream &input);
  std::vector<std::string> generate_file_names(boost::gregorian::date from,
                                               boost::gregorian::date to,
                                               std::string prefix,
                                               std::string suffix);
  boost::gregorian::date string_to_date(std::string d);
  std::map<std::string, std::string> parse_query_string(std::string query_string);
  
  /* int or(); */
  /* int not(); */
  /* int near(); */
  /* int and(); */
}

#endif

