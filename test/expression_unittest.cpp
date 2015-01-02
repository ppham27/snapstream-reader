
#include "Expression.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(constructor, Exceptions) {
  ASSERT_THROW(snap::Expression(""),
               snap::ExpressionSyntaxError);
  ASSERT_THROW(snap::Expression("china}"),
               snap::ExpressionSyntaxError);
  ASSERT_THROW(snap::Expression("{china"),
               snap::ExpressionSyntaxError);
}

TEST(constructor, Default) {
  snap::Expression e1 = snap::Expression("{expression}");
  ASSERT_EQ(e1.raw_expression, "{expression}");
  std::vector<std::pair<std::string, snap::TokenType>> v1;
  v1.emplace_back("expression", snap::TokenType::STRING);
  ASSERT_EQ(v1, e1.tokenized_expression);
  std::vector<std::string> p1{"expression"};
  ASSERT_EQ(p1, e1.patterns);

  snap::Expression e2 = snap::Expression("{expression0}@{expression1}");
  std::vector<std::pair<std::string, snap::TokenType>> v2;
  v2.emplace_back("expression0", snap::TokenType::STRING);
  v2.emplace_back("@", snap::TokenType::OPERATOR);
  v2.emplace_back("expression1", snap::TokenType::STRING);
  ASSERT_EQ(v2, e2.tokenized_expression);
  std::vector<std::string> p2{"expression0","expression1"};
  ASSERT_EQ(p2, e2.patterns);

  snap::Expression e3 = snap::Expression("({s0} + {s1})@100({s2} & {s3})");
  std::vector<std::pair<std::string, snap::TokenType>> v3;
  v3.emplace_back("(", snap::TokenType::OPERATOR);
  v3.emplace_back("s0", snap::TokenType::STRING);
  v3.emplace_back("+", snap::TokenType::OPERATOR);
  v3.emplace_back("s1", snap::TokenType::STRING);
  v3.emplace_back(")", snap::TokenType::OPERATOR);
  v3.emplace_back("@100", snap::TokenType::OPERATOR);
  v3.emplace_back("(", snap::TokenType::OPERATOR);
  v3.emplace_back("s2", snap::TokenType::STRING);
  v3.emplace_back("&", snap::TokenType::OPERATOR);
  v3.emplace_back("s3", snap::TokenType::STRING);
  v3.emplace_back(")", snap::TokenType::OPERATOR);
  ASSERT_EQ(v3, e3.tokenized_expression);
  std::vector<std::string> p3{"s0","s1","s2","s3"};
  ASSERT_EQ(p3, e3.patterns);
}

TEST(rpn, Default) {
  std::vector<snap::Expression> expressions;
  std::vector<std::queue<std::pair<std::string, snap::TokenType>>> expected_rpn;

  expressions.emplace_back("{expression}");
  expected_rpn.push_back(std::queue<std::pair<std::string, snap::TokenType>>());
  expected_rpn.back().emplace("expression", snap::TokenType::STRING);

  expressions.emplace_back("{expression0}@{expression1}");
  expected_rpn.push_back(std::queue<std::pair<std::string, snap::TokenType>>());
  expected_rpn.back().emplace("expression0", snap::TokenType::STRING);
  expected_rpn.back().emplace("expression1", snap::TokenType::STRING);
  expected_rpn.back().emplace("@", snap::TokenType::OPERATOR);

  expressions.emplace_back("({s0} + {s1})@100({s2} & {s3})");
  expected_rpn.push_back(std::queue<std::pair<std::string, snap::TokenType>>());
  expected_rpn.back().emplace("s0", snap::TokenType::STRING);
  expected_rpn.back().emplace("s1", snap::TokenType::STRING);
  expected_rpn.back().emplace("+", snap::TokenType::OPERATOR);
  expected_rpn.back().emplace("s2", snap::TokenType::STRING);
  expected_rpn.back().emplace("s3", snap::TokenType::STRING);
  expected_rpn.back().emplace("&", snap::TokenType::OPERATOR);
  expected_rpn.back().emplace("@100", snap::TokenType::OPERATOR);

  expressions.emplace_back("{s0}@100(({s1} & {s2}) !@20 {s3})");
  expected_rpn.push_back(std::queue<std::pair<std::string, snap::TokenType>>());
  expected_rpn.back().emplace("s0", snap::TokenType::STRING);
  expected_rpn.back().emplace("s1", snap::TokenType::STRING);
  expected_rpn.back().emplace("s2", snap::TokenType::STRING);
  expected_rpn.back().emplace("&", snap::TokenType::OPERATOR);
  expected_rpn.back().emplace("s3", snap::TokenType::STRING);
  expected_rpn.back().emplace("!@20", snap::TokenType::OPERATOR);
  expected_rpn.back().emplace("@100", snap::TokenType::OPERATOR);

  
  for (int i = 0; i < expressions.size(); ++i) {    
    auto eq = expected_rpn[i];
    auto aq = expressions[i].rpn();
    while (!eq.empty()) {
      ASSERT_EQ(eq.front(), aq.front());
      eq.pop(); aq.pop();
    }
    ASSERT_EQ(0, aq.size());
  }
  ASSERT_EQ(7, expressions.back().rpn().size());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
