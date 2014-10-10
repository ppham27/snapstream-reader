#ifndef SNAP_H
#define SNAP_H

#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "Program.h"
#include "Excerpt.h"

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::vector<std::string> &patterns,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(const std::string &pattern1,
                                               const std::string &pattern2,
                                               int distance,
                                               const std::string &s);
  
  namespace web {
    std::map<std::string, std::string> parse_query_string(const std::string &query_string);
    void print_header();
    void close_html();
    void print_excerpts(std::vector<snap::Excerpt> &excerpts, int n);
    void print_excerpt(const snap::Excerpt &e);
  }
  
  namespace io {
    bool file_exists(const std::string &file_name);
    std::vector<snap::Program> parse_programs(const std::string &file_name);
    std::vector<snap::Program> parse_programs(std::istream &input);
    std::vector<std::string> generate_file_names(boost::gregorian::date from,
                                                 boost::gregorian::date to,
                                                 std::string prefix,
                                                 std::string suffix);
  }
  namespace date {
    boost::gregorian::date string_to_date(std::string d);
    std::string date_to_string(boost::gregorian::date d);
  }
  /* int or(); */
  /* int not(); */
  /* int and(); */
}

#endif

