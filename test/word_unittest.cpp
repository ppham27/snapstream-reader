#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(tokenize, default) {
  std::string text = R"ZZZ(>>> Back we go to meteorologist, Karen Rogers with the latest from accuweather. Are you keeping your eye to the sky. 
>> One eye on the sky one on the radar, that is the way I play. Stormtracker 6 live double scan showing us right now that the heavy, steady rain from the morning
)ZZZ";
  std::vector<std::vector<std::string>> expected_phrases{std::vector<std::string>{"back", "we", "go", "to", "meteorologist"},
      std::vector<std::string>{"karen", "rogers", "with", "the", "lastest", "from", "accuweather"}};
  std::vector<std::vector<std::string>> actual_phrases = snap::word::tokenize(text);
  ASSERT_EQ(expected_phrases.size(), actual_phrases.size());
  for (int i = 0; i < expected_phrases.size(); ++i) {
    ASSERT_EQ(expected_phrases[i], actual_phrases[i]);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}

