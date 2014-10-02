#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(generate_file_names, Default) {
  boost::gregorian::date from(2014,2,27);
  boost::gregorian::date to(2014,3,3);
  std::vector<std::string> file_names = snap::generate_file_names(from, to,
                                                                  "Data/",".txt");
  ASSERT_EQ(4,file_names.size());
  std::vector<std::string> expected_file_names{"Data/2014-02-27.txt",
      "Data/2014-02-28.txt",
      "Data/2014-03-01.txt",
      "Data/2014-03-02.txt"};
  ASSERT_EQ(expected_file_names,
            file_names);
}

TEST(string_to_date, Default) {
  boost::gregorian::date d1(2014,5,7);
  boost::gregorian::date d2(2014,12,23);
  ASSERT_EQ(d1,
            snap::string_to_date("2014-05-07"));
  ASSERT_EQ(d1,
            snap::string_to_date("2014-5-7"));
  ASSERT_EQ(d2,
            snap::string_to_date("2014-12-23"));
}

TEST(parse_query_string, Default) {
  std::string query_string("search-string=phil&from-date=2014-03-02&to-date=2014-07-01");
  std::map<std::string, std::string> expected_kv;
  expected_kv["search-string"] = "phil";
  expected_kv["from-date"] = "2014-03-02";
  expected_kv["to-date"] = "2014-07-01";
  ASSERT_EQ(expected_kv, snap::parse_query_string(query_string));
}
