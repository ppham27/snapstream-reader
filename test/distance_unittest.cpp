#include <map>
#include <string>
#include <vector>

#include "distance.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"




TEST(filter_top, Default) {
  std::map<std::string, std::map<std::string, int>> M;
  M["A"] = std::map<std::string, int>();
  M["B"] = std::map<std::string, int>();
  M["C"] = std::map<std::string, int>();
  M["D"] = std::map<std::string, int>();
  M["E"] = std::map<std::string, int>();

  M["A"]["A"] = 10;
  M["A"]["B"] = 20;
  M["A"]["C"] = 30;
  M["A"]["D"] = 40;
  M["A"]["E"] = 50;

  M["B"]["B"] = 100;
  M["B"]["C"] = 90;
  M["B"]["D"] = 70;
  M["B"]["E"] = 80;

  M["C"]["C"] = 70;
  M["C"]["D"] = 60;
  M["C"]["E"] = 50;

  M["D"]["D"] = 20;
  M["D"]["E"] = 100;

  M["E"]["E"] = 21;

  // so we should only end up with B, C, and E 
  std::map<std::string, std::map<std::string, int>> N = distance::filter_top(M, 3);
  ASSERT_EQ(N.size(), 3);
  
  ASSERT_EQ(N.count("A"), 0);
  ASSERT_EQ(N.count("B"), 1);
  ASSERT_EQ(N.count("C"), 1);
  ASSERT_EQ(N.count("D"), 0);
  ASSERT_EQ(N.count("E"), 1);
  
  std::vector<std::string> keys{"B", "C", "E"};  
  for (auto it = keys.begin(); it != keys.end(); ++it) {
    for (auto jt(it); jt != keys.end(); ++jt) {
      ASSERT_EQ(N.at(*it).at(*jt), M.at(*it).at(*jt));
    }    
  }  
}

TEST(size_pow, Default) {
  std::map<std::string, std::map<std::string, int>> M;
  M["A"] = std::map<std::string, int>();
  M["B"] = std::map<std::string, int>();
  M["C"] = std::map<std::string, int>();
  M["D"] = std::map<std::string, int>();
  M["E"] = std::map<std::string, int>();
  M["A"]["A"] = 40;
  M["B"]["B"] = 700;
  M["C"]["C"] = 5;
  M["D"]["D"] = 1;
  M["E"]["E"] = 4;
  std::map<std::string, double> sizes = distance::size_pow(M, 1.0/3);
  ASSERT_NEAR(3.419952, sizes["A"], 1e-6);
  ASSERT_NEAR(8.87904, sizes["B"], 1e-6);
  ASSERT_NEAR(1.709976, sizes["C"], 1e-6);
  ASSERT_NEAR(1.0, sizes["D"], 1e-6);
  ASSERT_NEAR(1.587401, sizes["E"], 1e-6);
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
