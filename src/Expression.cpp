
#include <cctype>
#include <map>
#include <stack>

#include <iostream>

#include "Expression.h"

snap::Expression::Expression(const std::string &e) {
  if (e.length() == 0) { throw ExpressionSyntaxError(e, "Expression cannot be empty."); }
  this -> operator_precedence["!@"] = std::make_pair(2, false);
  this -> operator_precedence["@"] = std::make_pair(2, false);
  this -> operator_precedence["!&"] = std::make_pair(2, false);
  this -> operator_precedence["&"] = std::make_pair(2, false);
  this -> operator_precedence["+"] = std::make_pair(2, false);
  this -> raw_expression = e;
  this -> tokenized_expression = tokenize(e);
  for (auto it = (this -> tokenized_expression).begin(); it != (this -> tokenized_expression).end(); ++it) {
    if (it -> second == snap::TokenType::STRING) { (this -> patterns).push_back(it -> first); }
  }
  this -> rpn_expression = convert_to_rpn(this -> tokenized_expression);
}

std::vector<std::pair<std::string, snap::TokenType>> snap::Expression::tokenize(const std::string &e) {
  if (e.find("{") == -1) { throw ExpressionSyntaxError(e, "No beginning curly brace."); }
  std::vector<std::pair<std::string, snap::TokenType>> tokenized_expression;
  int idx = 0;  
  std::stack<char> parentheses_stack;
  while (idx < e.length()) {
    if (isspace(e[idx])) { ++idx; continue; }
    if (e[idx] == '{') {
      int next_idx = e.find("}", idx + 1);
      if (next_idx == -1) { throw ExpressionSyntaxError(e, "There is no closing curly brace."); }
      tokenized_expression.emplace_back(e.substr(idx+1, next_idx - idx - 1), snap::TokenType::STRING);
      idx = next_idx + 1;
    } else if (e[idx] == '(' || e[idx] == ')') {
      if (e[idx] == '(') { 
        parentheses_stack.push('('); 
      } else if (e[idx] == ')') {
        if (parentheses_stack.empty()) { throw ExpressionSyntaxError(e, "Mismatched parentheses."); }
        parentheses_stack.pop();
      }          
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
  if (!parentheses_stack.empty()) { throw ExpressionSyntaxError(e, "Mismatched parentheses."); }
  return tokenized_expression;
}

std::queue<std::pair<std::string, snap::TokenType>> snap::Expression::convert_to_rpn(const std::vector<std::pair<std::string, snap::TokenType>> &e) {
  // use shunting yard algorithm
  std::stack<std::pair<std::string, snap::TokenType>> s;
  std::queue<std::pair<std::string, snap::TokenType>> q;
  for (auto it = tokenized_expression.begin(); it != tokenized_expression.end(); ++it) {
    std::pair<std::string, snap::TokenType> token = *it;
    if (token.second == snap::TokenType::OPERATOR &&
        token.first != "(" && token.first != ")") {
      std::string operator_symbol;
      if (token.first.length() >= 2 && token.first.substr(0,2) == "!@") {
        operator_symbol = "!@";
      } else if (token.first.length() >= 1 && token.first[0] == '@') {
        operator_symbol = "@";
      } else {
        operator_symbol = token.first;
      }
      if (!operator_precedence.count(operator_symbol)) { 
        throw ExpressionSyntaxError(raw_expression, operator_symbol + " is not a recognized operator."); 
      }
      while (!s.empty() && s.top().second == snap::TokenType::OPERATOR && s.top().first != "(" && s.top().first != ")") {
        std::string next_operator_symbol;
        if (s.top().first.length() >= 2 && s.top().first.substr(0,2) == "!@") {
          next_operator_symbol = "!@";
        } else if (s.top().first.length() >= 1 && s.top().first[0] == '@') {
          next_operator_symbol = "@";
        } else {
          next_operator_symbol = s.top().first;
        }
        if ((!operator_precedence[next_operator_symbol].second &&
             operator_precedence[next_operator_symbol].first >= operator_precedence[operator_symbol].first) ||
            (operator_precedence[next_operator_symbol].second &&
             operator_precedence[next_operator_symbol].first > operator_precedence[operator_symbol].first)) {
          q.push(s.top()); s.pop();
        } else {          
          break;
        }
      }
      s.push(token);      
    } else if (token.first == "(") {
      s.push(token);
    } else if (token.first == ")") {
      while (s.top().first != "(") {
        q.push(s.top()); s.pop();        
      }      
      s.pop();
    } else {
      q.push(token);      
    }
  }
  while (!s.empty()) {
    q.push(s.top()); s.pop();    
  }
  return q;    
}

std::queue<std::pair<std::string, snap::TokenType>> snap::Expression::rpn() const {
  return this -> rpn_expression;
}
