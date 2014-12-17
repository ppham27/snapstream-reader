
#include "cctype"

#include "Expression.h"

snap::Expression::Expression(const std::string &e) {
  this -> raw_expression = e;
  this -> tokenized_expression = tokenize(e);
}

std::vector<std::pair<std::string, snap::TokenType>> snap::Expression::tokenize(const std::string &e) {
  std::vector<std::pair<std::string, snap::TokenType>> tokenized_expression;
  int idx = 0;
  while (idx < e.length()) {
    if (isspace(e[idx])) { ++idx; continue; }
    if (e[idx] == '{') {
      int next_idx = e.find("}", idx + 1);
      tokenized_expression.emplace_back(e.substr(idx+1, next_idx - idx - 1), snap::TokenType::STRING);
      idx = next_idx + 1;
    } else if (e[idx] == '(' || e[idx] == ')') {
      tokenized_expression.emplace_back(e.substr(idx, 1), snap::TokenType::OPERATOR);
      ++idx;
    } else {
      int next_idx = idx;
      while (!isspace(e[next_idx]) && e[next_idx] != '{' && !(next_idx != idx && e[next_idx] == '(')) {
        ++next_idx;      
      }
      tokenized_expression.emplace_back(e.substr(idx, next_idx - idx), snap::TokenType::OPERATOR);
      idx = next_idx;
    }
  }
  return tokenized_expression;
}
