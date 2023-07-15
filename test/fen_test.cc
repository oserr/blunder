#include "fen.h"

#include "gmock/gmock.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

TEST(ReadFen, NoPieces)
{
  auto state = read_fen("");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoPieces);
}

TEST(ReadFen, InvalidRow)
{
  auto state = read_fen("///////");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidRow);
}

TEST(ReadFen, MissingRows)
{
  auto state = read_fen("x");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::MissingRows);
}

TEST(ReadFen, WhiteAreNotLogical)
{
  auto state = read_fen("8/8/8/8/8/8/8/8");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::WhiteNotLogical);
}

TEST(ReadFen, BlackAreNotLogical)
{
  auto state = read_fen("8/8/8/8/8/8/P7/K7");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::BlackNotLogical);
}

TEST(ReadFen, NoColor)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoColor);
}

TEST(ReadFen, InvalidColor)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 y");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidColor);
}

TEST(ReadFen, NoCastling)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoCastling);
}

TEST(ReadFen, InvalidCastling)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w j");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidCastling);
}

TEST(ReadFen, NoEnPassant)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoEnPassant);
}

TEST(ReadFen, InvalidEnPassant)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk z3");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidEnPassant);
}

TEST(ReadFen, NoHalfMoveMissing)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk -");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoHalfMove);
}

TEST(ReadFen, InvalidHalfMove)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - lox");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidHalfMove);
}

TEST(ReadFen, NoFullMove)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - 0");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::NoFullMove);
}

TEST(ReadFen, InvalidFullMove)
{
  auto state = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - 0 lox");
  ASSERT_FALSE(state);
  EXPECT_EQ(state.error(), FenErr::InvalidFullMove);
}

TEST(ReadFen, InitNewGame)
{
  auto state = read_fen(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  ASSERT_TRUE(state);
  EXPECT_EQ(*state, BoardState::new_board());
}

TEST(ReadFen, PawnsAndKing)
{
  auto state = read_fen("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
  ASSERT_TRUE(state);

  auto white = state->white();
  EXPECT_EQ(white.full_set().count(), white.pawn().count() + 1);
  EXPECT_THAT(white.king(), EqualToSq(SqList{Sq::h3}));
  EXPECT_THAT(white.pawn(),
      EqualToSq(SqList{Sq::a3, Sq::b4, Sq::d5, Sq::e4, Sq::f3, Sq::h4}));
  EXPECT_FALSE(state->has_wk_castling());
  EXPECT_FALSE(state->has_wq_castling());

  auto black = state->black();
  EXPECT_EQ(black.full_set().count(), black.pawn().count() + 1);
  EXPECT_THAT(black.king(), EqualToSq(SqList{Sq::f7}));
  EXPECT_THAT(black.pawn(),
      EqualToSq(SqList{Sq::a4, Sq::b5, Sq::d6, Sq::e5, Sq::f4, Sq::h5}));
  EXPECT_FALSE(state->has_bk_castling());
  EXPECT_FALSE(state->has_bq_castling());

  EXPECT_EQ(state->hm_count(), 99);
  EXPECT_EQ(state->fm_count(), 50);
}
