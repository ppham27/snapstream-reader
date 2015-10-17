#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(tokenize, default) {
  std::string text = R"ZZZ(>>> Back we go to meteorologist, Karen Rogers with the latest from accuweather. Are you keeping your eye to the sky. 
>> One eye on the sky one on the radar, that is the way I play. Stormtracker 6 live double scan showing us right now that the heavy, steady rain from the morning
)ZZZ";
  std::vector<std::vector<std::pair<std::string, int>>> expected_phrases{std::vector<std::pair<std::string, int>>{std::make_pair("back",4), std::make_pair("meteorologist",18)},
      std::vector<std::pair<std::string, int>>{std::make_pair("karen",33), std::make_pair("rogers",39), std::make_pair("with",46), std::make_pair("the",51), std::make_pair("latest",55), std::make_pair("from",62), std::make_pair("accuweather",67)},
        std::vector<std::pair<std::string, int>>{std::make_pair("are",80), std::make_pair("you",84), std::make_pair("keeping",88), std::make_pair("your",96), std::make_pair("eye",101), std::make_pair("the",108), std::make_pair("sky",112)},
          std::vector<std::pair<std::string, int>>{std::make_pair("one",121), std::make_pair("eye",125), std::make_pair("the",132), std::make_pair("sky",136), std::make_pair("one",140), std::make_pair("the",147), std::make_pair("radar",151)},
            std::vector<std::pair<std::string, int>>{std::make_pair("that",158), std::make_pair("the",166), std::make_pair("way",170), std::make_pair("play",176)},
              std::vector<std::pair<std::string, int>>{std::make_pair("stormtracker",182)} ,
                std::vector<std::pair<std::string, int>>{std::make_pair("live",197), std::make_pair("double",202), std::make_pair("scan",209), std::make_pair("showing",214), std::make_pair("right",225), std::make_pair("now",231), std::make_pair("that",235), std::make_pair("the",240), std::make_pair("heavy",244)},
                  std::vector<std::pair<std::string, int>>{std::make_pair("steady",251), std::make_pair("rain",258), std::make_pair("from",263), std::make_pair("the",268), std::make_pair("morning",272)}};
  std::vector<std::vector<std::pair<std::string, int>>> actual_phrases = snap::word::tokenize(text);
  ASSERT_EQ(expected_phrases.size(), actual_phrases.size());
  for (int i = 0; i < expected_phrases.size(); ++i) {
    ASSERT_EQ(expected_phrases[i], actual_phrases[i]);
  }

  text = R"ZZZ((School bell ringing) Well, at least now there's plenty of room for your masculinity. Not that you'll need it. Shake it up! 
Chyna: It should've been like any other day. Compose a four-part symphonic masterpiece, gently decline Fletcher's advances, and then walk Cameron home from school. 
(Panting) I thought I knew who I was. I thought I knew who my friends were. But today was not like any other day. 
(Groaning) Nothing was like it normally is. region)ZZZ";
  // expected_phrases = std::vector<std::vector<std::string>>{std::vector<std::string>{"school", "bell", "ringing"},
  //                                                          std::vector<std::string>{"well"},
  //                                                          std::vector<std::string>{"least", "now", "there"},
  //                                                          std::vector<std::string>{"plenty", "room", "for", "your", "masculinity"},
  //                                                          std::vector<std::string>{"not", "that", "you"}, 
  //                                                          std::vector<std::string>{"need"},
  //                                                          std::vector<std::string>{"shake"},
  //                                                          std::vector<std::string>{"chyna"},
  //                                                          std::vector<std::string>{"should"},
  //                                                          std::vector<std::string>{"been", "like", "any", "other", "day"},
  //                                                          std::vector<std::string>{"compose", "four", "part", "symphonic", "masterpiece"},
  //                                                          std::vector<std::string>{"gently", "decline", "fletcher"},
  //                                                          std::vector<std::string>{"advances"},
  //                                                          std::vector<std::string>{"and", "then", "walk", "cameron", "home", "from", "school"},
  //                                                          std::vector<std::string>{"panting"},
  //                                                          std::vector<std::string>{"thought", "knew", "who", "was"},
  //                                                          std::vector<std::string>{"thought", "knew", "who", "friends", "were"},
  //                                                          std::vector<std::string>{"but", "today", "was", "not", "like", "any", "other", "day"},
  //                                                          std::vector<std::string>{"groaning"},
  //                                                          std::vector<std::string>{"nothing", "was", "like", "normally"},
  //                                                          std::vector<std::string>{"region"}};
  actual_phrases = snap::word::tokenize(text);
  // ASSERT_EQ(expected_phrases.size(), actual_phrases.size());
  // for (int i = 0; i < expected_phrases.size(); ++i) {
  //   ASSERT_EQ(expected_phrases[i], actual_phrases[i]);
  // }
}

// TEST(count_words, default) {
//   std::vector<std::vector<std::string>> phrases{
//     std::vector<std::string>{"philip", "chris", "person", "and"},
//       std::vector<std::string>{"chris", "chris", "new", "word"},
//         std::vector<std::string>{"person"},
//           std::vector<std::string>{"no", "matches", "here"},
//             std::vector<std::string>{"person", "and"}
//   };
//   std::map<std::string, int> word_counts = snap::word::count_words(phrases);
//   ASSERT_EQ(1, word_counts["philip"]);
//   ASSERT_EQ(3, word_counts["chris"]);
//   ASSERT_EQ(1, word_counts["new"]);
//   ASSERT_EQ(3, word_counts["person"]);
//   ASSERT_EQ(2, word_counts["and"]);
//   ASSERT_EQ(1, word_counts["no"]);
//   ASSERT_EQ(1, word_counts["matches"]);
//   ASSERT_EQ(1, word_counts["here"]);
// }

TEST(compare_word_counts, default) {
  std::map<std::string, int> word_count_a;
  std::map<std::string, int> word_count_b;
  word_count_a["philip"] = 5;
  word_count_a["chris"] = 3;
  word_count_a["patrick"] = 100;
  word_count_a["jobin"] = 10000;
  
  word_count_b["philip"] = 10;
  word_count_b["chris"] = 9;  
  word_count_b["tim"] = 100;  
  word_count_b["patrick"] = 199;  
  word_count_b["jobin"] = 55000;
  word_count_b["masato"] = 8;
  std::map<std::string, std::pair<int, int>> hot_words = snap::word::compare_word_counts(word_count_a,
                                                                                            word_count_b,
                                                                                            10, 2.0);
  ASSERT_FALSE(hot_words.count("chris")); // not enough occurences
  ASSERT_FALSE(hot_words.count("patrick")); // doesn't increase enough
  ASSERT_FALSE(hot_words.count("masato"));  // not enough occurrences
  ASSERT_EQ(5, hot_words["philip"].first);  // 
  ASSERT_EQ(10, hot_words["philip"].second);
  ASSERT_EQ(10000, hot_words["jobin"].first);  
  ASSERT_EQ(55000, hot_words["jobin"].second);
  ASSERT_EQ(0, hot_words["tim"].first);  
  ASSERT_EQ(100, hot_words["tim"].second);  
  word_count_a = std::map<std::string, int>();
  word_count_b = std::map<std::string, int>();
  word_count_b["philip"] = 240;
  word_count_a["philip"] = 100;
  word_count_b["chris"] = 239;
  word_count_a["chris"] = 100;
  word_count_b["television"] = 241;
  word_count_a["television"] = 100;
  hot_words = snap::word::compare_word_counts(word_count_a,
                                              word_count_b,
                                              10, 2.4);
  // ASSERT_TRUE(hot_words.count("philip"));
  ASSERT_TRUE(hot_words.count("television"));
  ASSERT_FALSE(hot_words.count("chris"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}

