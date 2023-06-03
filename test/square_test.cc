#include "square.h"

#include <array>
#include <set>
#include <string_view>

#include "gmock/gmock.h"

using namespace blunder;

constexpr std::array<Sq, 64> kSquares = {
  Sq::a1, Sq::b1, Sq::c1, Sq::d1, Sq::e1, Sq::f1, Sq::g1, Sq::h1,
  Sq::a2, Sq::b2, Sq::c2, Sq::d2, Sq::e2, Sq::f2, Sq::g2, Sq::h2,
  Sq::a3, Sq::b3, Sq::c3, Sq::d3, Sq::e3, Sq::f3, Sq::g3, Sq::h3,
  Sq::a4, Sq::b4, Sq::c4, Sq::d4, Sq::e4, Sq::f4, Sq::g4, Sq::h4,
  Sq::a5, Sq::b5, Sq::c5, Sq::d5, Sq::e5, Sq::f5, Sq::g5, Sq::h5,
  Sq::a6, Sq::b6, Sq::c6, Sq::d6, Sq::e6, Sq::f6, Sq::g6, Sq::h6,
  Sq::a7, Sq::b7, Sq::c7, Sq::d7, Sq::e7, Sq::f7, Sq::g7, Sq::h7,
  Sq::a8, Sq::b8, Sq::c8, Sq::d8, Sq::e8, Sq::f8, Sq::g8, Sq::h8
};

constexpr std::array<std::string_view, 64> kSquareStr = {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

TEST(ToUint, ConvertsSquareToUnsigned)
{
  for (unsigned i = 0; i < kSquares.size(); ++i) {
    EXPECT_EQ(ToUint(kSquares[i]), i);
  }
}

TEST(ToSq, ConvertsUnsignedToSquare)
{
  for (unsigned i = 0; i < kSquares.size(); ++i) {
    EXPECT_EQ(ToSq(i), kSquares[i]);
  }
}

TEST(ToBitBoard, ConvertsSquareToBitboard)
{
  for (unsigned i = 0; i < kSquares.size(); ++i) {
    EXPECT_EQ(ToBitBoard(kSquares[i]), 1ull << i);
  }
}

TEST(ToStr, ConvertsSquareToString)
{
  for (unsigned i = 0; i < kSquares.size(); ++i) {
    EXPECT_EQ(ToStr(kSquares[i]), kSquareStr[i]);
  }
}

TEST(ToSqStr, ConvertsUnsignedToSquareString)
{
  for (unsigned i = 0; i < kSquares.size(); ++i) {
    EXPECT_EQ(ToSqStr(i), kSquareStr[i]);
  }
}

TEST(ToSetOfSq, ConvertsBitBoardToSquares)
{
  auto squares = std::set<Sq>{Sq::c1, Sq::f1, Sq::h1};
  EXPECT_EQ(ToSetOfSq(0b10100100), squares);
}

TEST(ToBitBoard, ConvertsSquaresToBitBoard)
{
  BitBoard bb = 1 | (1 << 8) | (1 << 23) | (1ull << 63);
  EXPECT_EQ(ToBitBoard({Sq::a1, Sq::a2, Sq::h3, Sq::h8}), bb);
}
