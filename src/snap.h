#ifndef SNAP_H
#define SNAP_H

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <regex>
#include <vector>
#include <fstream>
#include <sstream>

#include "Program.h"

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(const std::string &pattern1,
                                               const std::string &pattern2,
                                               int distance,
                                               const std::string &s);
  std::vector<snap::Program *> parse_programs(const std::string &file_name);
  std::vector<snap::Program *> parse_programs(std::istream &input);
  /* int or(); */
  /* int not(); */
  /* int near(); */
  /* int and(); */
}

#endif

