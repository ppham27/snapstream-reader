#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(string_to_date, Default) {
  boost::gregorian::date d1(2014,5,7);
  boost::gregorian::date d2(2014,12,23);
  ASSERT_EQ(d1,
            snap::date::string_to_date("2014-05-07"));
  ASSERT_EQ(d1,
            snap::date::string_to_date("2014-5-7"));
  ASSERT_EQ(d2,
            snap::date::string_to_date("2014-12-23"));
  ASSERT_THROW(snap::date::string_to_date(""),
               snap::date::InvalidDateException);
  ASSERT_THROW(snap::date::string_to_date("this is not a date"),
               snap::date::InvalidDateException);
  ASSERT_THROW(snap::date::string_to_date("2014=12-01"),
               snap::date::InvalidDateException);
}

TEST(date_to_string, Default) {
  boost::gregorian::date d1(2014,5,7);
  boost::gregorian::date d2(2014,12,23);
  ASSERT_EQ("2014-05-07",
            snap::date::date_to_string(d1));
  ASSERT_EQ("2014-12-23",
            snap::date::date_to_string(d2));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
