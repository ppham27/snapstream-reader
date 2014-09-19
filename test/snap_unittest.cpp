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

TEST(Program, Default) {
  snap::Program prog(R"ZZZ(Title: CBS News Sunday Morning
Description: Singer Sarah McLachlan; White House conversations secretly recorded by President Richard Nixon; director and screenwriter Richard Linklater; hoarding.
Channel: 3 - KYW
Recorded On: 7/27/2014 9:00:00 AM
Original Air Date: 7/27/2014
Transcript Generated by SnapStream
==================================

Captioning made possible by Johnson & Johnson, where quality products for the American family have been a tradition for generations good morning, I am Charles Osgood and this is Sunday morning. Summertime is the season when countless Americans spend as much time as possible outside the house, and no wonder. All too many of them, the house that they are escaping is a full house. A house so full of stuff there is hardly any room for them. Peter, Rita braver will report our cover story. .. 
>> Has now been diagnosed as a mental disorder, hoarding. >> Up to 5 percent of Americans. 
)ZZZ");
  ASSERT_EQ("CBS News Sunday Morning", prog.title);
  ASSERT_EQ("Singer Sarah McLachlan; White House conversations secretly recorded by President Richard Nixon; director and screenwriter Richard Linklater; hoarding.", prog.description);
  ASSERT_EQ("3 - KYW", prog.channel);
  ASSERT_EQ("2014-07-27", prog.recorded_date);
  ASSERT_EQ("2014-07-27", prog.aired_date);
  ASSERT_EQ("Captioning made possible by Johnson & Johnson, where quality products for the American family have been a tradition for generations good morning, I am Charles Osgood and this is Sunday morning. Summertime is the season when countless Americans spend as much time as possible outside the house, and no wonder. All too many of them, the house that they are escaping is a full house. A house so full of stuff there is hardly any room for them. Peter, Rita braver will report our cover story. .. \n>> Has now been diagnosed as a mental disorder, hoarding. >> Up to 5 percent of Americans.", prog.text);

  snap::Program equal_prog(R"ZZZ(Title: CBS News Sunday Morning
Description: Singer Sarah McLachlan; White House conversations secretly recorded by President Richard Nixon; director and screenwriter Richard Linklater; hoarding.
Channel: 3 - KYW
Recorded On: 7/27/2014 9:00:00 AM
Original Air Date: 7/27/2014
Transcript Generated by SnapStream
==================================

Captioning made possible by Johnson & Johnson, where quality products for the American family have been a tradition for generations good morning, I am Charles Osgood and this is Sunday morning. Summertime is the season when countless Americans spend as much time as possible outside the house, and no wonder. All too many of them, the house that they are escaping is a full house. A house so full of stuff there is hardly any room for them. Peter, Rita braver will report our cover story. .. 
>> Has now been diagnosed as a mental disorder, hoarding. >> Up to 5 percent of Americans. 
)ZZZ");
  snap::Program inequal_prog(R"ZZZ(Title: CBS News Sunday Morning
Description: Singer Sarah McLachlan; White House conversations secretly recorded by President Richard Nixon; director and screenwriter Richard Linklater; hoarding.
Channel: 3 - KYW
Recorded On: 7/27/2014 9:00:00 AM
Original Air Date: 7/27/2014
Transcript Generated by SnapStream
==================================

blah bal afas
)ZZZ");
  ASSERT_EQ(prog, equal_prog);
  ASSERT_NE(prog, inequal_prog);
}

TEST(parse_programs, Default) {
  std::string s(R"ZZZ(

ï»¿Title: ABC World News Now
Description: Global news.
Channel: 6 - WPVI
Recorded On: 7/23/2014 2:11:00 AM
Original Air Date: 1/5/1992
Transcript Generated by SnapStream
==================================

,,  going to make this place your home ?????? 

[2:14:16 AM]

>> It every time we come back singing from a break. 
>> Off good voice. 
They've earned their wings. And you can, too. Together we can solve child hunger. Support feeding America and your local food bank at feedingamerica.org.

ï»¿Title: Your World With Neil Cavuto
Description: Money tips from Wall Street.
Channel: 360 - FNC
Recorded On: 7/23/2014 4:00:00 PM
Original Air Date: 10/7/1996
Transcript Generated by SnapStream
==================================

Hero before it became passe and awful like Facebook. Maybe we'll have something old from the '80s like all of us. Dow no good. See you later.  life. Like a snapshot, snapped then shot over and done. And those of us still alive just watch and wonder about last week and this week and almost too afraid to even think about next week. )ZZZ");
  std::istringstream iss(s);  
  snap::Program prog0(R"ZZZ(Title: ABC World News Now
Description: Global news.
Channel: 6 - WPVI
Recorded On: 7/23/2014 2:11:00 AM
Original Air Date: 1/5/1992
Transcript Generated by SnapStream
==================================

,,  going to make this place your home ?????? 

[2:14:16 AM]

>> It every time we come back singing from a break. 
>> Off good voice. 
They've earned their wings. And you can, too. Together we can solve child hunger. Support feeding America and your local food bank at feedingamerica.org.
)ZZZ");
  snap::Program prog1(R"ZZZ(Title: Your World With Neil Cavuto
Description: Money tips from Wall Street.
Channel: 360 - FNC
Recorded On: 7/23/2014 4:00:00 PM
Original Air Date: 10/7/1996
Transcript Generated by SnapStream
==================================

Hero before it became passe and awful like Facebook. Maybe we'll have something old from the '80s like all of us. Dow no good. See you later.  life. Like a snapshot, snapped then shot over and done. And those of us still alive just watch and wonder about last week and this week and almost too afraid to even think about next week. )ZZZ");
  std::vector<snap::Program> prog_vector = snap::parse_programs(iss);
  ASSERT_EQ(prog_vector.size(), 2);
  ASSERT_EQ(prog0, prog_vector[0]);
  ASSERT_EQ(prog1, prog_vector[1]);
}

TEST(find, Default) {
  ASSERT_THAT(snap::find("china","china is china is china")["china"],
              ::testing::ElementsAre(0, 9, 18));
  // match whole words only
  ASSERT_THAT(snap::find("china","achina is china is china")["china"],
              ::testing::ElementsAre(10, 19));
  ASSERT_THAT(snap::find("china","china is chinaa is china")["china"],
              ::testing::ElementsAre(0, 19));
  // take into account punctuation
  ASSERT_THAT(snap::find("china","?china is china.")["china"],
              ::testing::ElementsAre(1, 10));  
  // handle search strings with spaces
  ASSERT_THAT(snap::find("united kingdom","i live in the united kingdom")["united kingdom"],
              ::testing::ElementsAre(14));
  ASSERT_THAT(snap::find("united kingdom","i live in the lunited kingdom | united kingdom | united kingdomc | united kingdom.")["united kingdom"],
              ::testing::ElementsAre(32, 67));
  // handle wildcards
  ASSERT_THAT(snap::find("*united kingdom","i live in the lunited kingdom | united kingdom | united kingdomc | united kingdom.")["*united kingdom"],
              ::testing::ElementsAre(15, 32, 67));
  ASSERT_THAT(snap::find("united kingdom*","i live in the lunited kingdom | united kingdom | united kingdomc | united kingdom.")["united kingdom*"],
              ::testing::ElementsAre(32, 49, 67));
  ASSERT_THAT(snap::find("*united kingdom*","i live in the lunited kingdom | united kingdom | united kingdomc | united kingdom.")["*united kingdom*"],
              ::testing::ElementsAre(15, 32, 49, 67));
}

TEST(near, Default) {
  std::map<std::string, std::vector<int>> match_positions;
  std::string pattern1;
  std::string pattern2;
  int distance;
  std::string text;
  

  pattern1 = "china";
  pattern2 = "russia";
  distance = 15;
  text = "there are no matches here";
  ASSERT_EQ(match_positions[pattern1].size(), 0);
  ASSERT_EQ(match_positions[pattern2].size(), 0);
  match_positions = snap::near(pattern1, pattern2, distance, text);
  text = "together are china and russia blah blah blah blah blah china blah blah russia blah";
  match_positions = snap::near(pattern1, pattern2, distance, text);
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(13));
  ASSERT_THAT(match_positions[pattern2],
              ::testing::ElementsAre(23));
  distance = 20;
  match_positions = snap::near(pattern1, pattern2, distance, text);
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(13, 55));
  ASSERT_THAT(match_positions[pattern2],
              ::testing::ElementsAre(23, 71));
}


