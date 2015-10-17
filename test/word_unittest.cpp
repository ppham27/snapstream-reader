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
  expected_phrases = std::vector<std::vector<std::pair<std::string, int>>>{std::vector<std::pair<std::string, int>>{std::make_pair("school",1), std::make_pair("bell",8), std::make_pair("ringing",13)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("well",22)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("least",31), std::make_pair("now",37), std::make_pair("there",41)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("plenty",49), std::make_pair("room",59), std::make_pair("for",64), std::make_pair("your",68), std::make_pair("masculinity",73)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("not",86), std::make_pair("that",90), std::make_pair("you",95)}, 
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("need",102)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("shake",111)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("chyna",125)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("should",135)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("been",145), std::make_pair("like",150), std::make_pair("any",155), std::make_pair("other",159), std::make_pair("day",165)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("compose",170), std::make_pair("four",180), std::make_pair("part",185), std::make_pair("symphonic",190), std::make_pair("masterpiece",200)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("gently",213), std::make_pair("decline",220), std::make_pair("fletcher",228)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("advances",239)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("and",249), std::make_pair("then",253), std::make_pair("walk",258), std::make_pair("cameron",263), std::make_pair("home",271), std::make_pair("from",276), std::make_pair("school",281)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("panting",291)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("thought",302), std::make_pair("knew",312), std::make_pair("who",317), std::make_pair("was",323)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("thought",330), std::make_pair("knew",340), std::make_pair("who",345), std::make_pair("friends",352), std::make_pair("were",360)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("but",366), std::make_pair("today",370), std::make_pair("was",376), std::make_pair("not",380), std::make_pair("like",384), std::make_pair("any",389), std::make_pair("other",393), std::make_pair("day",399)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("groaning",406)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("nothing",416), std::make_pair("was",424), std::make_pair("like",428), std::make_pair("normally",436)},
                                                                           std::vector<std::pair<std::string, int>>{std::make_pair("region",449)}};
  actual_phrases = snap::word::tokenize(text);
  ASSERT_EQ(expected_phrases.size(), actual_phrases.size());
  for (int i = 0; i < expected_phrases.size(); ++i) {
    ASSERT_EQ(expected_phrases[i], actual_phrases[i]);
  }
}

TEST(count_words, default) {
  std::vector<std::vector<std::pair<std::string, int>>> phrases{
    std::vector<std::pair<std::string, int>>{std::make_pair("philip",-1), std::make_pair("chris",-1), std::make_pair("person",-1), std::make_pair("and",-1)},
      std::vector<std::pair<std::string, int>>{std::make_pair("chris",-1), std::make_pair("chris",-1), std::make_pair("new",-1), std::make_pair("word",-1)},
        std::vector<std::pair<std::string, int>>{std::make_pair("person",-1)},
          std::vector<std::pair<std::string, int>>{std::make_pair("no",-1), std::make_pair("matches",-1), std::make_pair("here",-1)},
            std::vector<std::pair<std::string, int>>{std::make_pair("person",-1), std::make_pair("and",-1)}
  };
  std::map<std::string, int> word_counts = snap::word::count_words(phrases);
  ASSERT_EQ(1, word_counts["philip"]);
  ASSERT_EQ(3, word_counts["chris"]);
  ASSERT_EQ(1, word_counts["new"]);
  ASSERT_EQ(3, word_counts["person"]);
  ASSERT_EQ(2, word_counts["and"]);
  ASSERT_EQ(1, word_counts["no"]);
  ASSERT_EQ(1, word_counts["matches"]);
  ASSERT_EQ(1, word_counts["here"]);
}

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

