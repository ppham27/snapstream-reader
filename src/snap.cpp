#include <algorithm>
#include <cmath>
#include <cctype>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "Program.h"


namespace snap {
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s) {
    std::string search_string;
    if (pattern.front() == '*' && pattern.back() == '*') {      
      search_string = pattern.substr(1, pattern.length() - 2);
    } else if (pattern.front() == '*') {
      search_string = pattern.substr(1, pattern.length() - 1);
    } else if (pattern.back() == '*') {
      search_string = pattern.substr(0, pattern.length() - 1);
    } else {
      search_string = pattern;
    }
    std::map<std::string, std::vector<int>> match_positions;
    std::vector<int> match_position;
    int current_position = 0;    
    int next_position;
    while ((next_position = s.find(search_string, current_position)) != -1) {
      current_position = next_position + search_string.length(); 
      if ((next_position == 0 || isspace(s[next_position-1]) || ispunct(s[next_position-1]) || pattern.front() == '*') &&
          (current_position == s.length() || isspace(s[current_position]) || ispunct(s[current_position]) || pattern.back() == '*')) {
        match_position.push_back(next_position);
      }
      // go to the next word
      while (!(current_position == s.length() ||
               isspace(s[current_position]) ||
               ispunct(s[current_position]))) {
        ++current_position;
      }
    }
    match_positions[pattern] = match_position;
    return match_positions;
  }

  std::map<std::string, std::vector<int>> near(const std::string &pattern1,
                                               const std::string &pattern2,
                                               int distance,
                                               const std::string &s) {
    std::vector<int> pattern1_locii = find(pattern1, s)[pattern1];
    std::vector<int> pattern2_locii = find(pattern2, s)[pattern2];    
    std::map<std::string, std::vector<int>> match_positions;
    match_positions[pattern1] = std::vector<int>();
    match_positions[pattern2] = std::vector<int>();
    if (pattern1_locii.size() == 0 || pattern2_locii.size() == 0) { return match_positions; }
    auto q = pattern2_locii.begin();
    for (int p : pattern1_locii) {
      while (q + 1 != pattern2_locii.end() && *q < p && p - *q > distance) { ++q; }
      if (abs(p - *q) <= distance) {
        match_positions[pattern1].push_back(p);
        match_positions[pattern2].push_back(*q);
        while (q + 1 != pattern2_locii.end() && abs(p-*(q+1)) <= distance) {
          ++q;
          match_positions[pattern2].push_back(*q);
        }
      }
    }
    return match_positions;
  }

  std::vector<snap::Program> parse_programs(const std::string &file_name) {
    std::ifstream ifs(file_name, std::ifstream::in);    
    return parse_programs(ifs);
  }
  
  std::vector<snap::Program> parse_programs(std::istream &input) {
    std::vector<snap::Program> prog_vector;
    const int read_size = 100000;
    char program_text[read_size];
    while (!input.eof()) {
      input.getline(program_text, read_size, -65);
      input.getline(program_text, read_size, -61);
      std::string program_string(program_text);
      prog_vector.emplace_back(program_string);
    }    
    return prog_vector;
  }

  std::string pad_number(int n, int p) {
    int digits = log10(n) + 1;
    std::string nstr = std::to_string(n);
    while (digits < p) {
      nstr = '0' + nstr; ++digits;
    }
    return nstr;
  }
  
  std::string date_to_string(boost::gregorian::date d) {
    boost::gregorian::date::ymd_type ymd = d.year_month_day();
    return pad_number(ymd.year, 4) + '-' + pad_number(ymd.month, 2) + '-' + pad_number(ymd.day, 2);
  }  
  std::vector<std::string> generate_file_names(boost::gregorian::date from,
                                               boost::gregorian::date to,
                                               std::string prefix,
                                               std::string suffix) {
    std::vector<std::string> file_names;
    while (from < to) {
      file_names.push_back(prefix + date_to_string(from) + suffix);
      from += boost::gregorian::date_duration(1);
    }
    return file_names;
  }
}


