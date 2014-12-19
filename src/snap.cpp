#include <algorithm>
#include <cctype>
#include <queue>
#include <stack>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "snap.h"
#include "Program.h"

#include <iostream>

namespace snap {


  std::vector<int> orv(const std::vector<int> &a, const std::vector<int> &b) {
    std::vector<int> c; 
    c.insert(c.end(), a.begin(), a.end());
    c.insert(c.end(), b.begin(), b.end());
    std::sort(c.begin(), c.end());
    return c;
  }
  
  std::vector<int> andv(const std::vector<int> &a, const std::vector<int> &b) {
    if (a.size() == 0 || b.size() == 0) {
      std::vector<int> c;
      return c;
    }
    return orv(a, b);
  }

  std::vector<int> notandv(const std::vector<int> &a, const std::vector<int> &b) {
    std::vector<int> c;
    if (b.size() == 0) {
      c.insert(c.end(), a.begin(), a.end());
    }
    return c;
  }

  std::vector<int> nearv(const std::vector<int> &a, const std::vector<int> &b, int distance) {
    std::vector<int> c;
    if (b.size() == 0) { return c; }
    auto bit = b.begin();
    for (auto ait = a.begin(); ait != a.end(); ++ait) {
      while (bit != b.end()-1 && *bit  < *ait && *ait - *bit > distance) {
        ++bit;
      }
      if (abs(*ait - *bit) <= distance) { c.push_back(*ait); }
    }
    return c;
  }
  
  std::vector<int> notnearv(const std::vector<int> &a, const std::vector<int> &b, int distance) {
    std::vector<int> c;
    if (b.size() == 0) {
      c.insert(c.end(), a.begin(), a.end());
      return c;
    }
    auto bit = b.begin();
    for (auto ait = a.begin(); ait != a.end(); ++ait) {
      while (bit != b.end()-1 && *bit  < *ait && *ait - *bit > distance) {
        ++bit;
      }
      if (abs(*ait - *bit) > distance) { c.push_back(*ait); }
    }
    return c;
  }

  std::vector<int> evaluate_expression(const snap::Expression &e, const std::map<std::string, std::vector<int>> &locations) {
    std::queue<std::pair<std::string,snap::TokenType>> rpn = e.rpn();
    std::stack<std::pair<std::string,snap::TokenType>> operands;
    std::map<std::string, std::vector<int>> new_locations;
    while (!rpn.empty()) {
      std::pair<std::string,snap::TokenType> token = rpn.front(); rpn.pop();
      if (token.second == TokenType::OPERATOR) {
        std::pair<std::string,snap::TokenType> b = operands.top(); operands.pop();
        std::pair<std::string,snap::TokenType> a = operands.top(); operands.pop();
        std::vector<int> aloci = locations.count(a.first) ? locations.at(a.first) : new_locations.at(a.first);
        std::vector<int> bloci = locations.count(b.first) ? locations.at(b.first) : new_locations.at(b.first);
        std::string new_key = "(" + a.first + " " + token.first + " " + b.first + ")";
        if (token.first == "&") {
          new_locations[new_key] = andv(aloci, bloci);
        } else if (token.first == "!&") {          
          new_locations[new_key] = notandv(aloci, bloci);
        } else if (token.first == "+") {
          new_locations[new_key] = orv(aloci, bloci);
        } else {
          // near cases
          int distance = 100;   // default distance
          if (token.first.substr(0,2) == "!@") {            
            if (token.first.length() > 2) { distance = stoi(token.first.substr(2)); }
            new_locations[new_key] = notnearv(aloci, bloci, distance);
          } else {
            if (token.first.length() > 1) { distance = stoi(token.first.substr(1)); }
            new_locations[new_key] = nearv(aloci, bloci, distance);
          }
        }
        operands.emplace(new_key, snap::TokenType::STRING);
      } else {
        operands.push(token);
      }        
    }    
    if (locations.count(operands.top().first)) {
      return locations.at(operands.top().first);
    } else {
      return new_locations[operands.top().first];
    }
  }

  std::map<std::string, std::vector<int>> evaluate_expressions(const std::vector<snap::Expression> &expressions,
                                                               const std::map<std::string, std::vector<int>> &locations) {
    std::map<std::string, std::vector<int>> res;
    for (auto e = expressions.begin(); e != expressions.end(); ++e) {
      res[e -> raw_expression] = evaluate_expression(*e, locations);
    }
    return res;    
  }
  
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
    
    // initialize by trimming and finding first instance
    int current_position = 0;
    int next_position;
    for (auto it = patterns.begin(); it != patterns.end(); ++it) {
      match_positions[*it] = std::vector<int>(0);
      search_strings[*it] = pattern_to_search_string(*it);
      next_position = s.find(search_strings[*it]);      
      if (next_position != -1) { next_positions.emplace(next_position, *it); }
    }
    // begin searching
    while (!next_positions.empty()) {
      std::pair<int, std::string> current_pair = next_positions.top();
      next_positions.pop();
      current_position = current_pair.first;
      std::string pattern = current_pair.second;      
      next_position = current_position + search_strings[pattern].length();
      // check for a valid match, if not wildcard, check for space/punct in front and back
      if ((current_position == 0 ||
           isspace(s[current_position-1]) ||
           ispunct(s[current_position-1]) ||
           pattern.front() == '*') &&
          (next_position == s.length() ||
           isspace(s[next_position]) ||
           ispunct(s[next_position]) ||
           pattern.back() == '*')) {        
        match_positions[pattern].push_back(current_position);
      }
      // move the current position after the word
      current_position = next_position;   
      while (!(current_position == s.length() ||
               isspace(s[current_position]) ||
               ispunct(s[current_position]))) {
        ++current_position;
      }
      //  find next instance and queue it
      next_position = s.find(search_strings[pattern], current_position);
      if (next_position != -1) {
        next_positions.emplace(next_position, pattern);
      }
    }
    return match_positions;
  }
  
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s) {
    return find(std::vector<std::string>{pattern}, s);
  }

  std::map<std::string, std::vector<int>> near(const snap::Expression &e1,
                                               const snap::Expression &e2,
                                               int distance,
                                               const std::string &s) {
    std::vector<std::string> patterns;
    patterns.insert(patterns.end(), e1.patterns.begin(), e1.patterns.end());
    patterns.insert(patterns.end(), e2.patterns.begin(), e2.patterns.end());
    std::map<std::string, std::vector<int>> pre_match_positions = find(patterns, s);
    std::vector<int> pattern1_locii = evaluate_expression(e1, pre_match_positions);
    std::vector<int> pattern2_locii = evaluate_expression(e2, pre_match_positions);
    std::map<std::string, std::vector<int>> match_positions;
    match_positions[e1.raw_expression] = std::vector<int>();
    match_positions[e2.raw_expression] = std::vector<int>();
    if (pattern1_locii.size() == 0 || pattern2_locii.size() == 0) { return match_positions; }
    auto q = pattern2_locii.begin();
    for (int p : pattern1_locii) {
      while (q + 1 != pattern2_locii.end() && *q < p && p - *q > distance) { ++q; }
      if (abs(p - *q) <= distance) {
        match_positions[e1.raw_expression].push_back(p);
        match_positions[e2.raw_expression].push_back(*q);
        while (q + 1 != pattern2_locii.end() && abs(p-*(q+1)) <= distance) {
          ++q;
          match_positions[e2.raw_expression].push_back(*q);
        }
      }
    }
    return match_positions;
  }

  std::map<std::string, std::map<std::string, int>> pair(const std::map<std::string, std::vector<int>> &match_positions,
                                                         int distance) {
    // initialize coocurrence count
    std::map<std::string, std::map<std::string, int>> cooccurences;
    for (auto it0 = match_positions.begin(); it0 != match_positions.end(); ++it0) {
      for (auto it1(it0); it1 != match_positions.end(); ++it1) {
        cooccurences[it0 -> first][it1 -> first] = 0;
      }
    }
    std::vector<std::pair<int, std::string>> positions;
    for (auto it0 = match_positions.begin(); it0 != match_positions.end(); ++it0) {
      for (auto it1 = (it0 -> second).begin(); it1 != (it0 -> second).end(); ++it1) {
        positions.emplace_back(*it1, it0 -> first);
      }
    }
    std::sort(positions.begin(), positions.end());
    for (auto it0 = positions.begin(); it0 != positions.end(); ++it0) {
      for (auto it1(it0); it1 >= positions.begin() && (it0 -> first) - (it1 -> first) <= distance; --it1) {
        if ((it0 -> second) <= (it1 -> second)) {
          ++cooccurences[it0 -> second][it1 -> second];
        } else {
          ++cooccurences[it1 -> second][it0 -> second];
        }
      }
    }
    return cooccurences;
  }
}


