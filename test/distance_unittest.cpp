#include <map>
#include <string>
#include <vector>

#include "distance.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>


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
  std::map<std::string, std::map<std::string, double>> Mdouble = distance::int_matrix_to_double_matrix(M);
  
  // so we should only end up with B, C, and E 
  std::map<std::string, std::map<std::string, double>> N = distance::filter_top(Mdouble, 3);
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

TEST(correlate_sum, Default) {
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

  std::map<std::string, std::map<std::string, double>> Mdouble = distance::int_matrix_to_double_matrix(M);
  
  std::map<std::string, std::map<std::string, double>> A = distance::correlate_sum(Mdouble);
  
  ASSERT_NEAR(0.9662069, A["C"]["D"], 1e-6);
  ASSERT_NEAR(0.5188889, A["A"]["B"], 1e-6);
  ASSERT_NEAR(0.3247315, A["E"]["E"], 1e-6);
  ASSERT_NEAR(1.6049948, A["D"]["E"], 1e-6);
  ASSERT_NEAR(1.0810185, A["B"]["B"], 1e-6);
  ASSERT_NEAR(1.2882759, A["A"]["D"], 1e-6);
  
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
  std::map<std::string, std::map<std::string, double>> Mdouble = distance::int_matrix_to_double_matrix(M);  
  std::map<std::string, double> sizes = distance::size_pow(Mdouble, 1.0/3);
  ASSERT_NEAR(3.419952, sizes["A"], 1e-6);
  ASSERT_NEAR(8.87904, sizes["B"], 1e-6);
  ASSERT_NEAR(1.709976, sizes["C"], 1e-6);
  ASSERT_NEAR(1.0, sizes["D"], 1e-6);
  ASSERT_NEAR(1.587401, sizes["E"], 1e-6);
}

TEST(distance_inv, Default) {
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
  std::map<std::string, std::map<std::string, double>> Mdouble = distance::int_matrix_to_double_matrix(M);  
  std::map<std::string, std::map<std::string, double>> N = distance::distance_inv(Mdouble, 1.0);
  std::map<std::string, std::map<std::string, double>> O = distance::distance_inv(Mdouble, 0.1);
  
  ASSERT_EQ(0, N["A"]["A"]);
  ASSERT_EQ(0, N["B"]["B"]);
  ASSERT_EQ(0, N["E"]["E"]);
  ASSERT_NEAR(1.0/(1+20), N["A"]["B"], 1e-6);
  ASSERT_NEAR(1.0/(0.1+20), O["A"]["B"], 1e-6);
  ASSERT_NEAR(1.0/(1+70), N["B"]["D"], 1e-6);
  ASSERT_NEAR(1.0/(0.1+70), O["B"]["D"], 1e-6);
  ASSERT_NEAR(1.0/(1+60), N["C"]["D"], 1e-6);
  ASSERT_NEAR(1.0/(0.1+60), O["C"]["D"], 1e-6);
  ASSERT_NEAR(1.0/(1+100), N["D"]["E"], 1e-6);
  ASSERT_NEAR(1.0/(0.1+100), O["D"]["E"], 1e-6);
}

TEST(size_distance_to_csv, Default) {
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

  std::map<std::string, std::map<std::string, double>> Mdouble = distance::int_matrix_to_double_matrix(M);  
  std::map<std::string, double> sizes = distance::size_pow(Mdouble, 1.0/3);
  std::map<std::string, std::map<std::string, double>> distances = distance::distance_inv(Mdouble, 1);
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
