
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(pair, Default) {
  std::string text = "There is therefore now no condemnation for those who are in Christ Jesus. For the law of the Spirit of life has set you free in Christ Jesus from the law of sin and death. For God has done what the law, weakened by the flesh, could not do. By sending his own Son in the likeness of sinful flesh and for sin, he condemned sin in the flesh, in order that the righteous requirement of the law might be fulfilled in us, who walk not according to the flesh but according to the Spirit. For those who live according to the flesh set their minds on the things of the flesh, but those who live according to the Spirit set their minds on the things of the Spirit. For to set the mind on the flesh is death, but to set the mind on the Spirit is life and peace. For the mind that is set on the flesh is hostile to God, for it does not submit to God’s law; indeed, it cannot. Those who are in the flesh cannot please God";
  std::string lower_text(text);
  std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
  std::vector<std::string> patterns{"there","condemn*","spirit",
      "sin*", "death", "god", "submit", "flesh", "christ"};
  std::map<std::string, std::vector<int>> match_positions = snap::find(patterns, lower_text);
  ASSERT_THAT(match_positions["there"],
              ::testing::ElementsAre(0));
  ASSERT_THAT(match_positions["condemn*"],
              ::testing::ElementsAre(26, 311));
  ASSERT_THAT(match_positions["spirit"],
              ::testing::ElementsAre(93, 473, 603, 647, 725));
  ASSERT_THAT(match_positions["sin*"],
              ::testing::ElementsAre(157, 282, 303, 321));
  ASSERT_THAT(match_positions["death"],
              ::testing::ElementsAre(165, 691));
  ASSERT_THAT(match_positions["god"],
              ::testing::ElementsAre(176, 803, 907));
  ASSERT_THAT(match_positions["submit"],
              ::testing::ElementsAre(824));
  ASSERT_THAT(match_positions["flesh"],
              ::testing::ElementsAre(219, 289, 332, 446, 517, 560, 682, 783, 887));
  ASSERT_THAT(match_positions["christ"],
              ::testing::ElementsAre(60, 128));
  
  std::map<std::string, std::map<std::string, int>> low_distance_cooccurences = snap::pair(match_positions, 50);
  std::map<std::string, std::map<std::string, int>> middle_distance_cooccurences = snap::pair(match_positions, 100);
  std::map<std::string, std::map<std::string, int>> long_distance_cooccurences = snap::pair(match_positions, 150);
  ASSERT_EQ(9, low_distance_cooccurences.size());
  ASSERT_EQ(1, low_distance_cooccurences["condemn*"]["there"]);
  ASSERT_EQ(1, long_distance_cooccurences["condemn*"]["there"]);
  ASSERT_EQ(0, low_distance_cooccurences["christ"]["there"]);
  ASSERT_EQ(1, middle_distance_cooccurences["christ"]["there"]);
  ASSERT_EQ(2, long_distance_cooccurences["christ"]["there"]);
  ASSERT_EQ(3, low_distance_cooccurences["flesh"]["god"]);
  ASSERT_EQ(4, middle_distance_cooccurences["flesh"]["god"]);
  ASSERT_EQ(2, low_distance_cooccurences["christ"]["spirit"]);
}

TEST(find, Multi) {
  std::map<std::string, std::vector<int>> match_positions0 = snap::find(std::vector<std::string>{"china","is"}, "china is china is china");
  ASSERT_THAT(match_positions0["china"],
              ::testing::ElementsAre(0, 9, 18));
  ASSERT_THAT(match_positions0["is"],
              ::testing::ElementsAre(6, 15));
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

  pattern1 = "{china}";
  pattern2 = "{russia}";
  snap::Expression e1(pattern1);
  snap::Expression e2(pattern2);  

  distance = 15;
  text = "there are no matches here";
  ASSERT_EQ(match_positions[pattern1].size(), 0);
  ASSERT_EQ(match_positions[pattern2].size(), 0);
  match_positions = snap::near(e1, e2, distance, text);
  text = "together are china and russia blah blah blah blah blah china blah blah russia blah";
  match_positions = snap::near(e1, e2, distance, text);
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(13));
  ASSERT_THAT(match_positions[pattern2],
              ::testing::ElementsAre(23));
  distance = 20;
  match_positions = snap::near(e1, e2, distance, text);
  ASSERT_THAT(match_positions[pattern1],
              ::testing::ElementsAre(13, 55));
  ASSERT_THAT(match_positions[pattern2],
              ::testing::ElementsAre(23, 71));
}

TEST(filter_program_list, Default) {
  std::vector<std::string> program_list{"newszz", "news", 
      "abcde", "abc", "action news", 
      "saturday", "friday", "monday", "zdaynotimporantstuff",
      "zday"};
  std::vector<std::string> expected_filtered_program_list{"abc", 
      "action news", 
      "friday",
      "monday",
      "news",
      "saturday",
      "zday"};
  std::vector<std::string> filtered_program_list = snap::filter_program_list(program_list);
  ASSERT_EQ(expected_filtered_program_list, filtered_program_list);  
}

TEST(is_program_selected, Default) {
  std::vector<std::string> program_list{"abc", 
      "action news", 
      "friday",
      "monday",
      "news",
      "saturday",
      "zday"};
  ASSERT_TRUE(snap::is_program_selected("news", program_list));
  ASSERT_FALSE(snap::is_program_selected("mon", program_list));
  ASSERT_TRUE(snap::is_program_selected("monday1234", program_list));
  ASSERT_FALSE(snap::is_program_selected("aaaa", program_list));
  ASSERT_TRUE(snap::is_program_selected("zdayzz", program_list));
  ASSERT_TRUE(snap::is_program_selected("zdayef", program_list));
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
