#ifndef SNAP_H
#define SNAP_H

#include <cctype>
#include <map>
#include <string>
#include <regex>
#include <vector>

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(const std::string &pattern1,
                                               const std::string &pattern2,
                                               int distance,
                                               const std::string &s);
  /* int or(); */
  /* int not(); */
  /* int near(); */
  /* int and(); */
}

#endif

