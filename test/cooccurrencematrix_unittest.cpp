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
  std::string program = R"ZZZ(
0000000000 china 1111111111111111111111 22222222222222 russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
0000000000 china 1111111111111111111111 22222222222222 russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
0000000000 china 1111111111111111111111 22222222222222 russia 333333333333333333333
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(1,std::get<0>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(1,std::get<1>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(3,std::get<2>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(1,std::get<0>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(1,std::get<1>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(3,std::get<2>(matrix.at("{china}")["{russia}"]));

  // reversing the order is a new context
  program = R"ZZZ(
22222222222222 russia 333333333333333333333 0000000000 china 1111111111111111111111 
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(1,std::get<0>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(2,std::get<1>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(4,std::get<2>(matrix.at("{china}")["{china}"]));
  ASSERT_EQ(2,std::get<0>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(2,std::get<1>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(4,std::get<2>(matrix.at("{china}")["{russia}"]));

  // add new programs but don't add a new context
  program = R"ZZZ(
0000000000 china 1111111111111111111111 22222222222222 russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
000000000x china 1111111111111111111111 2222222222222x russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
000000000x china 1111111111111111111111 22222222222222 russia x33333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
0000000000 china x111111111111111111111 2222222222222x russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
0000000000 china x111111111111111111111 2222222222222x russia x33333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(2,std::get<0>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(7,std::get<1>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(9,std::get<2>(matrix.at("{china}")["{russia}"]));

  // add new contexts
  program = R"ZZZ(
00000000x0 china 1x11111111111111111111 22222222222222 russia 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
0000000000 china 1111111111111111111111 222222222222x2 russia 3x3333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  program = R"ZZZ(
0000000x00 china 11x1111111111111111111 22222222222x22 russia 33x333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(5,std::get<0>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(10,std::get<1>(matrix.at("{china}")["{russia}"]));
  ASSERT_EQ(12,std::get<2>(matrix.at("{china}")["{russia}"]));

  // handle nears properly
  program = R"ZZZ(
0000000000 iraq syria 1111111111111111111111 22222222222222 united states 333333333333333333333
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
4444444444 iraq syria 5555555555555555555555 66666666666666 united states 777777777777777777777
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(1,std::get<0>(matrix.at("{iraq} @ {syria}")["{united states}"]));
  ASSERT_EQ(1,std::get<1>(matrix.at("{iraq} @ {syria}")["{united states}"]));     
  ASSERT_EQ(2,std::get<2>(matrix.at("{iraq} @ {syria}")["{united states}"]));     

  // only add program
  program = R"ZZZ(
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
4444444444 iraq syria 5555555555555555555555 66666666666666 united states 777777777777777777777
)ZZZ";
  matrix.add_program(program, distance); 
  ASSERT_EQ(1,std::get<0>(matrix.at("{iraq} @ {syria}")["{united states}"]));
  ASSERT_EQ(2,std::get<1>(matrix.at("{iraq} @ {syria}")["{united states}"]));     
  ASSERT_EQ(3,std::get<2>(matrix.at("{iraq} @ {syria}")["{united states}"]));     
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
