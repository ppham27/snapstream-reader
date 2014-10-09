#include <algorithm>
#include <cctype>
#include <queue>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "snap.h"
#include "Program.h"


namespace snap {
  
  std::string pattern_to_search_string(const std::string &pattern) {
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
    return search_string;
  }
  
  std::map<std::string, std::vector<int>> find(const std::vector<std::string> &patterns,
                                               const std::string &s) {
    std::map<std::string, std::vector<int>> match_positions;
    std::map<std::string, std::string> search_strings;
    std::priority_queue<std::pair<int, std::string>, std::vector<std::pair<int, std::string>>,
                        std::greater<std::pair<int, std::string>>> next_positions;
    int current_position = 0;
    int next_position;
    for (auto it = patterns.begin(); it != patterns.end(); ++it) {
      match_positions[*it] = std::vector<int>(0);
      search_strings[*it] = pattern_to_search_string(*it);
      next_position = s.find(search_strings[*it], current_position);
      next_positions.emplace(next_position, *it);
    }
    while (!next_positions.empty()) {
      std::pair<int, std::string> current_pair = next_positions.top();
      next_positions.pop();
    }
    return match_positions;
  }
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s) {
    std::string search_string = pattern_to_search_string(pattern);
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
}


