#include "fen.h"

#include "gmock/gmock.h"

using namespace blunder;

TEST(ReadFen, NoPieces)
{
  auto state = ReadFen("");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoPieces);
}

TEST(ReadFen, InvalidRow)
{
  auto state = ReadFen("///////");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidRow);
}

TEST(ReadFen, MissingRows)
{
  auto state = ReadFen("x");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::MissingRows);
}

TEST(ReadFen, WhiteAreNotLogical)
{
  auto state = ReadFen("8/8/8/8/8/8/8/8");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::WhiteNotLogical);
}

TEST(ReadFen, BlackAreNotLogical)
{
  auto state = ReadFen("8/8/8/8/8/8/P7/K7");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::BlackNotLogical);
}

TEST(ReadFen, NoColor)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoColor);
}

TEST(ReadFen, InvalidColor)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 y");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidColor);
}

TEST(ReadFen, NoCastling)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoCastling);
}

TEST(ReadFen, InvalidCastling)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w j");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidCastling);
}

TEST(ReadFen, NoEnPassant)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoEnPassant);
}

TEST(ReadFen, InvalidEnPassant)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk z3");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidEnPassant);
}

TEST(ReadFen, NoHalfMoveMissing)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk -");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoHalfMove);
}

TEST(ReadFen, InvalidHalfMove)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk - lox");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidHalfMove);
}

TEST(ReadFen, NoFullMove)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk - 0");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoFullMove);
}

TEST(ReadFen, InvalidFullMove)
{
  auto state = ReadFen("k7/p7/8/8/8/8/P7/K7 w Kk - 0 lox");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidFullMove);
}

TEST(ReadFen, InitNewGame)
{
  auto state = ReadFen(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  ASSERT_TRUE(state);
  EXPECT_EQ(*state, NewBoardState());
}

TEST(ReadFen, PawnsAndKing)
{
  auto state = ReadFen("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
  ASSERT_TRUE(state);
  EXPECT_EQ(*state, NewBoardState());
}
