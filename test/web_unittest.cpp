#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(parse_query_string, Default) {
  std::string query_string0("search-string=phil&from-date=2014-03-02&to-date=2014-07-01");
  std::string query_string1("search-string=united+states&from-date=2014-09-03&to-date=2014-09-05");
  std::map<std::string, std::string> expected_kv;
  expected_kv["search-string"] = "phil";
  expected_kv["from-date"] = "2014-03-02";
  expected_kv["to-date"] = "2014-07-01";
  ASSERT_EQ(expected_kv, snap::web::parse_query_string(query_string0));
  expected_kv["search-string"] = "united states";
  expected_kv["from-date"] = "2014-09-03";
  expected_kv["to-date"] = "2014-09-05";
  ASSERT_EQ(expected_kv, snap::web::parse_query_string(query_string1));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
