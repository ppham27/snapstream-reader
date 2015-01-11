#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cstring>
#include <exception>
#include <string>
#include <map>
#include <queue>
#include <vector>

namespace snap {

  enum TokenType { STRING, OPERATOR };
  
  class Expression {    
  public:
      // first is precendence, second is associativity
    std::map<std::string, std::pair<int, bool>> operator_precedence;
    std::string raw_expression;
    std::vector<std::pair<std::string, snap::TokenType>> tokenized_expression;
    std::vector<std::string> patterns;
    Expression(const std::string &e);
    std::vector<std::pair<std::string, snap::TokenType>> tokenize(const std::string &e);
    std::queue<std::pair<std::string, snap::TokenType>> convert_to_rpn(const std::vector<std::pair<std::string, snap::TokenType>> &e);
    std::queue<std::pair<std::string, snap::TokenType>> rpn() const;
  private:
    std::queue<std::pair<std::string, snap::TokenType>> rpn_expression;
  };

  class ExpressionSyntaxError: public std::runtime_error {
  private:
    std::string e; std::string msg;
  public:
    ExpressionSyntaxError(std::string e) : std::runtime_error(" is not a valid expression."), e(e) { msg = ""; }
    ExpressionSyntaxError(std::string e, std::string msg) : std::runtime_error(" is not a valid expression."), e(e), msg(msg) {}
    virtual const char* what() const noexcept {
      std::string error_message(std::runtime_error::what());
      error_message = e + error_message;
      if (!msg.empty()) { error_message += " " + msg; }
      char *error_msg_c_str = new char[error_message.length() + 1];
      strcpy(error_msg_c_str, error_message.c_str());
      return error_msg_c_str;
    }
  };
}

#endif
