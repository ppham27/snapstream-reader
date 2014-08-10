#include <string>
#include <vector>

#include "snap.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(find, Default) {
  ASSERT_THAT(snap::find("china","china is china"), ::testing::ElementsAre(0,9));
}
