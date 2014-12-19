#ifndef SNAP_H
#define SNAP_H

#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "Program.h"
#include "Excerpt.h"
#include "Expression.h"

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::vector<std::string> &patterns,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(snap::Expression &pattern1,
                                               snap::Expression &pattern2,
                                               int distance,
                                               const std::string &s);

  std::map<std::string, std::map<std::string, int>> pair(const std::map<std::string, std::vector<int>> &match_positions,
                                                         int distance);

  std::string convert_infix_to_rpn(std::string expression);

  std::vector<int> andv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> notandv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> orv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> nearv(const std::vector<int> &a, const std::vector<int> &b, int distance);
  std::vector<int> notnearv(const std::vector<int> &a, const std::vector<int> &b, int distance);

  std::vector<int> evaluate_expression(snap::Expression &e, std::map<std::string, std::vector<int>> &locations);
  std::map<std::string, std::vector<int>> evaluate_expressions(std::vector<snap::Expression> &expressions, std::map<std::string, std::vector<int>> &locations);
  
  namespace web {
    std::map<std::string, std::string> parse_query_string(const std::string &query_string);
    void print_header();
    void close_html();
    void print_excerpts(std::vector<snap::Excerpt> &excerpts, int n);
    void print_excerpt(const snap::Excerpt &e);
    void print_matrix(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results);
    std::string sanitize_string(std::string s);
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

}

#endif

