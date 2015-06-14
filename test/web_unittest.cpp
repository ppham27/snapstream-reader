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

TEST(parse_multiform_data, Default) {
  std::string content_type1("multipart/form-data; boundary=----WebKitFormBoundaryZJ1caIw5vYNf0vwv");
  std::string body1 = R"ZZZ(------WebKitFormBoundaryZJ1caIw5vYNf0vwv
Content-Disposition: form-data; name="matrix_file"; filename="tmp.txt"
Content-Type: text/plain

hello, world!

------WebKitFormBoundaryZJ1caIw5vYNf0vwv
Content-Disposition: form-data; name="other_file"; filename="bye.txt"
Content-Type: text/plain

goodebye, cruel world...

------WebKitFormBoundaryZJ1caIw5vYNf0vwv--)ZZZ";
  std::string content_type2("multipart/form-data; boundary=----WebKitFormBoundary8wkTw0rFXBx7zXbe");
  std::string body2 = R"ZZZ(------WebKitFormBoundary8wkTw0rFXBx7zXbe
Content-Disposition: form-data; name="matrix_file"; filename="2014-08-01_matrix_1033111095.csv"
Content-Type: text/csv

472,18,52
18,1085,70
52,70,414

------WebKitFormBoundary8wkTw0rFXBx7zXbe
Content-Disposition: form-data; name="other_file"; filename="2014-08-01_keys_1030102642.csv"
Content-Type: text/csv

{china}
{iraq}
{russia}

------WebKitFormBoundary8wkTw0rFXBx7zXbe--)ZZZ";
  std::map<std::string, std::string> form1 = snap::web::parse_multiform_data(content_type1, body1);
  ASSERT_EQ("hello, world!", form1.at("matrix_file"));
  ASSERT_EQ("goodebye, cruel world...", form1.at("other_file"));
  std::map<std::string, std::string> form2 = snap::web::parse_multiform_data(content_type2, body2);
  ASSERT_EQ("472,18,52\n18,1085,70\n52,70,414", form2.at("matrix_file"));
  ASSERT_EQ("{china}\n{iraq}\n{russia}", form2.at("other_file"));
}

TEST(matrix_to_json, Default) {
  std::string matrix = "\nUSA,United States,85,-1,94,31\nGBR,Great Britain,40,94,-1,57\nRUS,Russia,70,31,57,-1\n\n\n";
  std::string json = R"ZZZ({"nodes":[{"symbol":"USA","name":"United States","size":85},{"symbol":"GBR","name":"Great Britain","size":40},{"symbol":"RUS","name":"Russia","size":70}],"times":[{"name":"1","key":"1"}],"links":[[{"1":-1,"k":-1},{"1":94,"k":1},{"1":31,"k":1}],[{"1":94,"k":1},{"1":-1,"k":-1},{"1":57,"k":1}],[{"1":31,"k":1},{"1":57,"k":1},{"1":-1,"k":-1}]]})ZZZ";
  ASSERT_EQ(json, snap::web::matrix_to_json(matrix));    
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}


