
#include <algorithm>

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(andv, default) {
  std::vector<int> a1{1,10,12};
  std::vector<int> b1{2,3,11};
  std::vector<int> a2;
  std::vector<int> b2;
  ASSERT_THAT(snap::andv(a1,b1),
              ::testing::ElementsAre(1,2,3,10,11,12));
  ASSERT_EQ(0, snap::andv(a2,b1).size());
  ASSERT_EQ(0, snap::andv(a1,b2).size());
}

TEST(orv, default) {
  std::vector<int> a1{1,10,12};
  std::vector<int> b1{2,3,11};
  ASSERT_THAT(snap::orv(a1,b1),
              ::testing::ElementsAre(1,2,3,10,11,12));
}

TEST(notandv, default) {
  std::vector<int> a1{1,10,12};
  std::vector<int> b1{2,3,11};
  std::vector<int> a2;
  std::vector<int> b2;
  ASSERT_EQ(0, snap::notandv(a2,b1).size());
  ASSERT_EQ(0, snap::notandv(a1,b1).size());
  ASSERT_THAT(snap::notandv(a1,b2),
              ::testing::ElementsAre(1,10,12));
}

TEST(nearv, default) {
  std::vector<int> a1{1,50,100};
  std::vector<int> b1{10,75,120};
  std::vector<int> b2;
  ASSERT_EQ(0, snap::nearv(a1,b2,5).size());
  ASSERT_EQ(0, snap::nearv(a1,b1,5).size());
  ASSERT_THAT(snap::nearv(a1,b1,10),
              ::testing::ElementsAre(1));
  ASSERT_THAT(snap::nearv(a1,b1,20),
              ::testing::ElementsAre(1,100));
  ASSERT_THAT(snap::nearv(a1,b1,25),
              ::testing::ElementsAre(1,50,100));
}

TEST(notnearv, default) {
  std::vector<int> a1{1,50,100};
  std::vector<int> b1{10,75,120};
  std::vector<int> b2;
  ASSERT_THAT(snap::notnearv(a1,b2,5),
              ::testing::ElementsAre(1,50,100));
  ASSERT_THAT(snap::notnearv(a1,b1,5),
              ::testing::ElementsAre(1,50,100));
  ASSERT_THAT(snap::notnearv(a1,b1,10),
              ::testing::ElementsAre(50,100));
  ASSERT_THAT(snap::notnearv(a1,b1,20),
              ::testing::ElementsAre(50));
  ASSERT_EQ(0, snap::notnearv(a1,b1,25).size());
}

TEST(evaluate_expression, default) {
  std::string text = "There is therefore now no condemnation for those who are in Christ Jesus. For the law of the Spirit of life has set you free in Christ Jesus from the law of sin and death. For God has done what the law, weakened by the flesh, could not do. By sending his own Son in the likeness of sinful flesh and for sin, he condemned sin in the flesh, in order that the righteous requirement of the law might be fulfilled in us, who walk not according to the flesh but according to the Spirit. For those who live according to the flesh set their minds on the things of the flesh, but those who live according to the Spirit set their minds on the things of the Spirit. For to set the mind on the flesh is death, but to set the mind on the Spirit is life and peace. For the mind that is set on the flesh is hostile to God, for it does not submit to God’s law; indeed, it cannot. Those who are in the flesh cannot please God";
  std::string lower_text(text);
  std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
  std::vector<std::string> patterns{"there","condemn*","spirit",
      "sin*", "death", "god", "submit", "flesh", "christ","jesus","lolidunno"};
  std::map<std::string, std::vector<int>> match_positions = snap::find(patterns, lower_text);

  snap::Expression e0("{sin*}");
  std::vector<int> a0 = evaluate_expression(e0, match_positions);
  ASSERT_THAT(a0,
              ::testing::ElementsAre(157, 282, 303, 321));  
  
  snap::Expression e1("{god} & {lolidunno}");
  std::vector<int> a1 = evaluate_expression(e1, match_positions);
  ASSERT_EQ(0, a1.size());
  
  snap::Expression e2("{god} & {christ}");
  std::vector<int> a2 = evaluate_expression(e2, match_positions);
  ASSERT_THAT(a2,
              ::testing::ElementsAre(60, 128, 176, 803, 907));

  snap::Expression e3("{sin*} @ {death}");
  std::vector<int> a3 = evaluate_expression(e3, match_positions);
  ASSERT_THAT(a3,
              ::testing::ElementsAre(157));

  snap::Expression e4("{sin*} !@ {death}");
  std::vector<int> a4 = evaluate_expression(e4, match_positions);
  ASSERT_THAT(a4,
              ::testing::ElementsAre(282, 303, 321));

  snap::Expression e5("{sin*} @150 {death}");
  std::vector<int> a5 = evaluate_expression(e5, match_positions);
  ASSERT_THAT(a5,
              ::testing::ElementsAre(157,282,303));

  snap::Expression e6("{sin*} !@150 {death}");
  std::vector<int> a6 = evaluate_expression(e6, match_positions);
  ASSERT_THAT(a6,
              ::testing::ElementsAre(321));

  snap::Expression e7("{god} + {christ}");
  std::vector<int> a7 = evaluate_expression(e7, match_positions);
  ASSERT_THAT(a7,
              ::testing::ElementsAre(60, 128, 176, 803, 907));

  snap::Expression e8("{christ} @50 ({sin*} @ {death})");
  std::vector<int> a8 = evaluate_expression(e8, match_positions);
  ASSERT_THAT(a8,
              ::testing::ElementsAre(128));

  snap::Expression e9("({christ} @ {spirit}) @ ({christ} @50 ({sin*} @ {death}))");
  std::vector<int> a9 = evaluate_expression(e9, match_positions);
  ASSERT_THAT(a9,
              ::testing::ElementsAre(60,128));

  std::vector<snap::Expression> expressions{e9,e2,e3};
  ASSERT_EQ(a9, evaluate_expressions(expressions, match_positions)[e9.raw_expression]);
  ASSERT_EQ(a2, evaluate_expressions(expressions, match_positions)[e2.raw_expression]);
  ASSERT_EQ(a3, evaluate_expressions(expressions, match_positions)[e3.raw_expression]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
