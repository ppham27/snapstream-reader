
#include "Expression.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>


TEST(constructor, Default) {
  snap::Expression e1 = snap::Expression("{expression}");
  ASSERT_EQ(e1.raw_expression, "{expression}");
  std::vector<std::pair<std::string, snap::TokenType>> v1;
  v1.emplace_back("expression", snap::TokenType::STRING);
  ASSERT_EQ(v1, e1.tokenized_expression);

  snap::Expression e2 = snap::Expression("{expression0}@{expression1}");
  std::vector<std::pair<std::string, snap::TokenType>> v2;
  v2.emplace_back("expression0", snap::TokenType::STRING);
  v2.emplace_back("@", snap::TokenType::OPERATOR);
  v2.emplace_back("expression1", snap::TokenType::STRING);
  ASSERT_EQ(v2, e2.tokenized_expression);

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
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
