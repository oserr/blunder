#include "moves.h"

#include "gtest/gtest.h"

using namespace blunder;

TEST(MoveNorth, ShiftsLeftBy8) {
   EXPECT_EQ(MoveNorth(1ull), 1ull << 8);
}

TEST(MoveSouth, ShiftsRightBy8) {
   EXPECT_EQ(MoveSouth(1024ull), 1024ull >> 8);
}
