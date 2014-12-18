#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <queue>
#include <vector>

namespace snap {

  enum TokenType { STRING, OPERATOR };
  
  class Expression {
  public:
    std::string raw_expression;
    std::vector<std::pair<std::string, snap::TokenType>> tokenized_expression;
    std::vector<std::string> patterns;
    Expression(const std::string &e);
    std::vector<std::pair<std::string, snap::TokenType>> tokenize(const std::string &e);
    std::queue<std::pair<std::string, snap::TokenType>> convert_to_rpn(const std::vector<std::pair<std::string, snap::TokenType>> &e);
    std::queue<std::pair<std::string, snap::TokenType>> rpn();
  private:
    std::queue<std::pair<std::string, snap::TokenType>> rpn_expression;
  };
}

#endif
