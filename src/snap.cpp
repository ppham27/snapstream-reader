#include "snap.h"
#include <cctype>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <iostream>


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
    std::vector<int> pattern1_loci = find(pattern1, s)[pattern1];
    std::vector<int> pattern2_loci = find(pattern2, s)[pattern2];    
    std::map<std::string, std::vector<int>> match_positions;
    match_positions[pattern1] = std::vector<int>();
    match_positions[pattern2] = std::vector<int>();
    if (pattern1_loci.size() == 0 || pattern2_loci.size() == 0) { return match_positions; }    
    return match_positions;
  }
}


