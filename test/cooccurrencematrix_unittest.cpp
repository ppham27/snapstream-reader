#include <string>
#include <vector>
#include <tuple>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Expression.h"
#include "CoOccurrenceMatrix.h"

const int A = 3;
const int M = 65071;
const int LEFT_HASH_WIDTH = 15;
const int RIGHT_HASH_WIDTH = 25;

TEST(CoOccurrenceMatrix, Constructor) {
  std::vector<snap::Expression> expressions;
  expressions.emplace_back("{china}");
  expressions.emplace_back("{united states}");
  expressions.emplace_back("{russia}");
  expressions.emplace_back("{iraq} @ {syria}");
  snap::CoOccurrenceMatrix matrix(expressions, M, A, LEFT_HASH_WIDTH, RIGHT_HASH_WIDTH);
  matrix.test();
}

TEST(CoOccurrenceMatrix, at) {
  std::vector<snap::Expression> expressions;
  expressions.emplace_back("{china}");
  expressions.emplace_back("{united states}");
  expressions.emplace_back("{russia}");
  expressions.emplace_back("{iraq} @ {syria}");
  snap::CoOccurrenceMatrix matrix(expressions, M, A, LEFT_HASH_WIDTH, RIGHT_HASH_WIDTH);
  ASSERT_EQ(0, std::get<0>(matrix.at("{china}")["{russia}"]));
  std::get<0>(matrix.at("{china}")["{russia}"]) += 2;
  ASSERT_EQ(2, std::get<0>(matrix.at("{china}")["{russia}"]));
  std::get<0>(matrix.at("{china}")["{russia}"]) += 2;
  ASSERT_EQ(4, std::get<0>(matrix.at("{china}")["{russia}"]));
}

TEST(CoOccurrenceMatrix, add_program) {
  int distance = 50;
  std::vector<snap::Expression> expressions;
  expressions.emplace_back("{china}");
  expressions.emplace_back("{united states}");
  expressions.emplace_back("{russia}");
  expressions.emplace_back("{iraq} @ {syria}");
  snap::CoOccurrenceMatrix matrix(expressions, M, A, LEFT_HASH_WIDTH, RIGHT_HASH_WIDTH);
  // initial add, only add once per program and once per context
  matrix.add_program("china", distance);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
