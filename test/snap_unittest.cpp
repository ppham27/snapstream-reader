#include <map>
#include <string>
#include <vector>

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

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
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(12));
  text = "together are china and russia blah blah blah blah blah china blah blah russia blah";
  match_positions = snap::near(pattern1, pattern2, distance, text);                         
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(12));
  ASSERT_THAT(match_positions[pattern2],
              ::testing::ElementsAre(23));
}
