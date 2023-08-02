#include "fen.h"

#include <memory>
#include <mutex>

#include "gmock/gmock.h"
#include "magic_attacks.h"
#include "pre_computed_magics.h"
#include "square.h"
#include "utils.h"

using namespace blunder;

// Only init magic bitboards once because it's expensive to init the magics.
std::once_flag init_flag;

class ReadFenTest : public testing::Test
{
protected:
  void
  SetUp() override
  {
    std::call_once(init_flag, []{
      auto bmagics = from_bmagics(kBishopMagics);
      auto rmagics = from_rmagics(kRookMagics);
      ASSERT_TRUE(bmagics) << "Unable to init bishop magics for MoveGen.";
      ASSERT_TRUE(rmagics) << "Unable to init rook magics for MoveGen.";
      Board::register_magics(
          std::make_unique<MagicAttacks>(std::move(*bmagics)),
          std::make_unique<MagicAttacks>(std::move(*rmagics)));
    });
  }
};

TEST_F(ReadFenTest, NoPieces)
{
  auto board = read_fen("");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoPieces);
}

TEST_F(ReadFenTest, InvalidRow)
{
  auto board = read_fen("///////");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidRow);
}

TEST_F(ReadFenTest, MissingRows)
{
  auto board = read_fen("x");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::MissingRows);
}

TEST_F(ReadFenTest, WhiteAreNotLogical)
{
  auto board = read_fen("8/8/8/8/8/8/8/8");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::WhiteNotLogical);
}

TEST_F(ReadFenTest, BlackAreNotLogical)
{
  auto board = read_fen("8/8/8/8/8/8/P7/K7");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::BlackNotLogical);
}

TEST_F(ReadFenTest, NoColor)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoColor);
}

TEST_F(ReadFenTest, InvalidColor)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 y");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidColor);
}

TEST_F(ReadFenTest, NoCastling)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoCastling);
}

TEST_F(ReadFenTest, InvalidCastling)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w j");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidCastling);
}

TEST_F(ReadFenTest, NoEnPassant)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoEnPassant);
}

TEST_F(ReadFenTest, InvalidEnPassant)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk z3");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidEnPassant);
}

TEST_F(ReadFenTest, NoHalfMoveMissing)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk -");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoHalfMove);
}

TEST_F(ReadFenTest, InvalidHalfMove)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - lox");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidHalfMove);
}

TEST_F(ReadFenTest, NoFullMove)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - 0");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::NoFullMove);
}

TEST_F(ReadFenTest, InvalidFullMove)
{
  auto board = read_fen("k7/p7/8/8/8/8/P7/K7 w Kk - 0 lox");
  ASSERT_FALSE(board);
  EXPECT_EQ(board.error(), FenErr::InvalidFullMove);
}

TEST_F(ReadFenTest, InitNewGame)
{
  auto board = read_fen(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  ASSERT_TRUE(board);
  EXPECT_EQ(*board, Board::new_board());
}

TEST_F(ReadFenTest, PawnsAndKing)
{
  auto board = read_fen("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50");
  ASSERT_TRUE(board);

  auto white = board->white();
  EXPECT_EQ(white.full_set().count(), white.pawn().count() + 1);
  EXPECT_THAT(white.king(), EqualToSq(SqList{Sq::h3}));
  EXPECT_THAT(white.pawn(),
      EqualToSq(SqList{Sq::a3, Sq::b4, Sq::d5, Sq::e4, Sq::f3, Sq::h4}));
  EXPECT_FALSE(board->has_wk_castling());
  EXPECT_FALSE(board->has_wq_castling());

  auto black = board->black();
  EXPECT_EQ(black.full_set().count(), black.pawn().count() + 1);
  EXPECT_THAT(black.king(), EqualToSq(SqList{Sq::f7}));
  EXPECT_THAT(black.pawn(),
      EqualToSq(SqList{Sq::a4, Sq::b5, Sq::d6, Sq::e5, Sq::f4, Sq::h5}));
  EXPECT_FALSE(board->has_bk_castling());
  EXPECT_FALSE(board->has_bq_castling());

  EXPECT_EQ(board->hm_count(), 99);
  EXPECT_EQ(board->fm_count(), 50);
}
