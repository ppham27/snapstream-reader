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
  std::string content_type3("multipart/form-data; boundary=----WebKitFormBoundaryT7T8JbOJxwmAPk3x");
  std::string body3 = R"ZZZ(------WebKitFormBoundaryT7T8JbOJxwmAPk3x
Content-Disposition: form-data; name="graph_title"

new title
------WebKitFormBoundaryT7T8JbOJxwmAPk3x
Content-Disposition: form-data; name="matrix_file"; filename="may.csv"
Content-Type: text/csv

USA,{united states} + {united states of america} + {usa},10.28,0.20,5.39,3.83,4.11,6.24,3.59,7.21,5.07,8.41,2.69,2.72,5.08,3.66,1.59,4.69,3.17,5.78,4.11,2.36,6.95
MEX,{mexico},7.70,5.39,0.08,10.00,7.45,10.00,6.79,6.84,5.19,10.00,1.94,10.00,10.00,10.00,4.43,5.97,5.77,10.00,1.90,10.00,10.00
IRQ,{iraq},7.67,3.83,10.00,0.10,5.92,0.77,10.00,10.00,10.00,10.00,10.00,4.61,0.27,4.57,10.00,10.00,10.00,10.00,10.00,0.88,3.21
PRC,{china} !@ {fine},7.30,4.11,7.45,5.92,0.07,7.04,4.74,10.00,10.00,6.55,4.80,1.74,5.97,10.00,4.04,1.53,1.89,3.29,1.25,3.32,4.49
AFG,{afghanistan},7.18,6.24,10.00,0.77,7.04,0.07,10.00,10.00,10.00,10.00,10.00,10.00,5.85,10.00,10.00,10.00,10.00,10.00,3.23,4.86,10.00
AUL,{australia},6.55,3.59,6.79,10.00,4.74,10.00,0.05,5.72,2.10,10.00,2.50,5.16,5.17,10.00,10.00,2.34,2.96,10.00,4.20,10.00,10.00
UKG,({united kingdom} + {scotland} + {wales} + ({england} !@ {new england})),6.60,7.21,6.84,10.00,10.00,10.00,5.72,0.05,3.13,10.00,5.77,5.22,10.00,3.50,10.00,4.84,2.23,1.27,10.00,10.00,10.00
FRA,{france},6.60,5.07,5.19,10.00,10.00,10.00,2.10,3.13,0.06,10.00,5.76,3.53,5.23,1.33,3.33,1.89,0.87,0.83,4.25,4.23,10.00
NEP,{nepal},6.66,8.41,10.00,10.00,6.55,10.00,10.00,10.00,10.00,0.05,10.00,10.00,10.00,10.00,10.00,3.25,10.00,10.00,1.32,10.00,10.00
CAN,{canada},6.60,2.69,1.94,10.00,4.80,10.00,2.50,5.77,5.76,10.00,0.06,3.53,10.00,10.00,5.00,3.18,3.00,1.53,4.25,4.23,10.00
RUS,{russia},6.13,2.72,10.00,4.61,1.74,10.00,5.16,5.22,3.53,10.00,3.53,0.05,2.26,2.22,2.86,1.58,1.86,3.67,3.72,1.64,10.00
SYR,{syria},6.14,5.08,10.00,0.27,5.97,5.85,5.17,10.00,5.23,10.00,10.00,2.26,0.06,10.00,10.00,10.00,10.00,1.05,10.00,0.62,10.00
ITA,{italy},6.09,3.66,10.00,4.57,10.00,10.00,10.00,3.50,1.33,10.00,10.00,2.22,10.00,0.04,10.00,10.00,2.52,0.87,3.68,10.00,10.00
CUB,{cuba},5.95,1.59,4.43,10.00,4.04,10.00,10.00,10.00,3.33,10.00,5.00,2.86,10.00,10.00,0.04,2.55,10.00,10.00,10.00,10.00,10.00
JAP,{japan},5.82,4.69,5.97,10.00,1.53,10.00,2.34,4.84,1.89,3.25,3.18,1.58,10.00,10.00,2.55,0.04,1.64,1.99,10.00,3.35,2.92
GER,{germany},5.66,3.17,5.77,10.00,1.89,10.00,2.96,2.23,0.87,10.00,3.00,1.86,10.00,2.52,10.00,1.64,0.04,1.03,3.18,10.00,10.00
SPA,{spain},5.35,5.78,10.00,10.00,3.29,10.00,10.00,1.27,0.83,10.00,1.53,3.67,1.05,0.87,10.00,1.99,1.03,0.04,0.73,10.00,10.00
INA,{india},5.38,4.11,1.90,10.00,1.25,3.23,4.20,10.00,4.25,1.32,4.25,3.72,10.00,3.68,10.00,10.00,3.18,0.73,0.03,10.00,10.00
IRN,{iran},5.37,2.36,10.00,0.88,3.32,4.86,10.00,10.00,4.23,10.00,4.23,1.64,0.62,10.00,10.00,3.35,10.00,10.00,10.00,0.03,10.00
VIE,{vietnam},5.03,6.95,10.00,3.21,4.49,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,2.92,10.00,10.00,10.00,10.00,0.02

------WebKitFormBoundaryT7T8JbOJxwmAPk3x--)ZZZ";
  std::string expected_matrix_file = R"ZZZ(USA,{united states} + {united states of america} + {usa},10.28,0.20,5.39,3.83,4.11,6.24,3.59,7.21,5.07,8.41,2.69,2.72,5.08,3.66,1.59,4.69,3.17,5.78,4.11,2.36,6.95
MEX,{mexico},7.70,5.39,0.08,10.00,7.45,10.00,6.79,6.84,5.19,10.00,1.94,10.00,10.00,10.00,4.43,5.97,5.77,10.00,1.90,10.00,10.00
IRQ,{iraq},7.67,3.83,10.00,0.10,5.92,0.77,10.00,10.00,10.00,10.00,10.00,4.61,0.27,4.57,10.00,10.00,10.00,10.00,10.00,0.88,3.21
PRC,{china} !@ {fine},7.30,4.11,7.45,5.92,0.07,7.04,4.74,10.00,10.00,6.55,4.80,1.74,5.97,10.00,4.04,1.53,1.89,3.29,1.25,3.32,4.49
AFG,{afghanistan},7.18,6.24,10.00,0.77,7.04,0.07,10.00,10.00,10.00,10.00,10.00,10.00,5.85,10.00,10.00,10.00,10.00,10.00,3.23,4.86,10.00
AUL,{australia},6.55,3.59,6.79,10.00,4.74,10.00,0.05,5.72,2.10,10.00,2.50,5.16,5.17,10.00,10.00,2.34,2.96,10.00,4.20,10.00,10.00
UKG,({united kingdom} + {scotland} + {wales} + ({england} !@ {new england})),6.60,7.21,6.84,10.00,10.00,10.00,5.72,0.05,3.13,10.00,5.77,5.22,10.00,3.50,10.00,4.84,2.23,1.27,10.00,10.00,10.00
FRA,{france},6.60,5.07,5.19,10.00,10.00,10.00,2.10,3.13,0.06,10.00,5.76,3.53,5.23,1.33,3.33,1.89,0.87,0.83,4.25,4.23,10.00
NEP,{nepal},6.66,8.41,10.00,10.00,6.55,10.00,10.00,10.00,10.00,0.05,10.00,10.00,10.00,10.00,10.00,3.25,10.00,10.00,1.32,10.00,10.00
CAN,{canada},6.60,2.69,1.94,10.00,4.80,10.00,2.50,5.77,5.76,10.00,0.06,3.53,10.00,10.00,5.00,3.18,3.00,1.53,4.25,4.23,10.00
RUS,{russia},6.13,2.72,10.00,4.61,1.74,10.00,5.16,5.22,3.53,10.00,3.53,0.05,2.26,2.22,2.86,1.58,1.86,3.67,3.72,1.64,10.00
SYR,{syria},6.14,5.08,10.00,0.27,5.97,5.85,5.17,10.00,5.23,10.00,10.00,2.26,0.06,10.00,10.00,10.00,10.00,1.05,10.00,0.62,10.00
ITA,{italy},6.09,3.66,10.00,4.57,10.00,10.00,10.00,3.50,1.33,10.00,10.00,2.22,10.00,0.04,10.00,10.00,2.52,0.87,3.68,10.00,10.00
CUB,{cuba},5.95,1.59,4.43,10.00,4.04,10.00,10.00,10.00,3.33,10.00,5.00,2.86,10.00,10.00,0.04,2.55,10.00,10.00,10.00,10.00,10.00
JAP,{japan},5.82,4.69,5.97,10.00,1.53,10.00,2.34,4.84,1.89,3.25,3.18,1.58,10.00,10.00,2.55,0.04,1.64,1.99,10.00,3.35,2.92
GER,{germany},5.66,3.17,5.77,10.00,1.89,10.00,2.96,2.23,0.87,10.00,3.00,1.86,10.00,2.52,10.00,1.64,0.04,1.03,3.18,10.00,10.00
SPA,{spain},5.35,5.78,10.00,10.00,3.29,10.00,10.00,1.27,0.83,10.00,1.53,3.67,1.05,0.87,10.00,1.99,1.03,0.04,0.73,10.00,10.00
INA,{india},5.38,4.11,1.90,10.00,1.25,3.23,4.20,10.00,4.25,1.32,4.25,3.72,10.00,3.68,10.00,10.00,3.18,0.73,0.03,10.00,10.00
IRN,{iran},5.37,2.36,10.00,0.88,3.32,4.86,10.00,10.00,4.23,10.00,4.23,1.64,0.62,10.00,10.00,3.35,10.00,10.00,10.00,0.03,10.00
VIE,{vietnam},5.03,6.95,10.00,3.21,4.49,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,10.00,2.92,10.00,10.00,10.00,10.00,0.02)ZZZ";
  std::map<std::string, std::string> form1 = snap::web::parse_multiform_data(content_type1, body1);
  ASSERT_EQ("hello, world!", form1.at("matrix_file"));
  ASSERT_EQ("goodebye, cruel world...", form1.at("other_file"));
  std::map<std::string, std::string> form2 = snap::web::parse_multiform_data(content_type2, body2);
  ASSERT_EQ("472,18,52\n18,1085,70\n52,70,414", form2.at("matrix_file"));
  ASSERT_EQ("{china}\n{iraq}\n{russia}", form2.at("other_file"));
  std::map<std::string, std::string> form3 = snap::web::parse_multiform_data(content_type3, body3);
  ASSERT_EQ("new title", form3.at("graph_title")); 
  ASSERT_EQ(expected_matrix_file, form3.at("matrix_file"));
}

TEST(matrix_to_json, Default) {
  std::string matrix = "\nUSA,United States,85,-1,94,31\nGBR,Great Britain,40,94,-1,57\nRUS,Russia,70,31,57,-1\n\n\n";
  std::string json = R"ZZZ({"nodes":[{"symbol":"USA","name":"United States","size":{"1":85}},{"symbol":"GBR","name":"Great Britain","size":{"1":40}},{"symbol":"RUS","name":"Russia","size":{"1":70}}],"times":[{"name":"1","key":"1"}],"links":[[{"1":-1,"k":-1},{"1":94,"k":1},{"1":31,"k":1}],[{"1":94,"k":1},{"1":-1,"k":-1},{"1":57,"k":1}],[{"1":31,"k":1},{"1":57,"k":1},{"1":-1,"k":-1}]]})ZZZ";
  ASSERT_EQ(json, snap::web::matrix_to_json(matrix));    
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}


