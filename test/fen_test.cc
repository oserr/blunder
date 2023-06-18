#include "fen.h"

#include "gmock/gmock.h"
#include "square.h"
#include "utils.h"

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

  EXPECT_EQ(state->NumWhite(), state->NumWhitePawn() + 1);
  EXPECT_THAT(state->WhiteKing(), EqualToSq(SqList{Sq::h3}));
  EXPECT_THAT(state->WhitePawn(),
      EqualToSq(SqList{Sq::a3, Sq::b4, Sq::d5, Sq::e4, Sq::f3, Sq::h4}));
  EXPECT_FALSE(state->wk_castle);
  EXPECT_FALSE(state->wq_castle);

  EXPECT_EQ(state->NumBlack(), state->NumBlackPawn() + 1);
  EXPECT_THAT(state->BlackKing(), EqualToSq(SqList{Sq::f7}));
  EXPECT_THAT(state->BlackPawn(),
      EqualToSq(SqList{Sq::a4, Sq::b5, Sq::d6, Sq::e5, Sq::f4, Sq::h5}));
  EXPECT_FALSE(state->bk_castle);
  EXPECT_FALSE(state->bq_castle);

  EXPECT_EQ(state->half_move, 99);
  EXPECT_EQ(state->full_move, 50);
}
