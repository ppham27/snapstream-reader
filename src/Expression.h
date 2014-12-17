#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>

namespace snap {

  enum TokenType { STRING, OPERATOR };
  
  class Expression {
  public:
    std::string raw_expression;
    std::vector<std::pair<std::string, snap::TokenType>> tokenized_expression;
    Expression(const std::string &e);
    std::vector<std::pair<std::string, snap::TokenType>> tokenize(const std::string &e);
  };
}

#endif
