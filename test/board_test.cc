#include "board.h"

#include <memory>
#include <mutex>
#include <vector>

#include "fen.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "magic_attacks.h"

using namespace blunder;

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

// Only init MoveGen once because it's expensive to init the magics.
std::once_flag init_flag;

class BoardTest : public testing::Test
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

TEST_F(BoardTest, InitGamePawnMoves)
{
  MoveVec moves;
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a3);
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a4);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b3);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b4);
  moves.emplace_back(Piece::pawn(), Sq::c2, Sq::c3);
  moves.emplace_back(Piece::pawn(), Sq::c2, Sq::c4);
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d3);
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d4);
  moves.emplace_back(Piece::pawn(), Sq::e2, Sq::e3);
  moves.emplace_back(Piece::pawn(), Sq::e2, Sq::e4);
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f3);
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f4);
  moves.emplace_back(Piece::pawn(), Sq::g2, Sq::g3);
  moves.emplace_back(Piece::pawn(), Sq::g2, Sq::g4);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h3);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h4);

  auto board = Board::new_board();
  EXPECT_THAT(board.pawn_moves(), UnorderedElementsAreArray(moves));
}

TEST_F(BoardTest, InitGameKingMoves)
{
  auto board = Board::new_board();
  EXPECT_THAT(board.king_moves(), IsEmpty());
}

TEST_F(BoardTest, InitGameKnightMoves)
{
  MoveVec moves;
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::a3);
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::c3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::f3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::h3);

  auto board = Board::new_board();
  EXPECT_THAT(board.knight_moves(), UnorderedElementsAreArray(moves));
}

TEST_F(BoardTest, InitGameQueenMoves)
{
  auto board = Board::new_board();
  EXPECT_THAT(board.queen_moves(), IsEmpty());
}

TEST_F(BoardTest, InitGameRookMoves)
{
  auto board = Board::new_board();
  EXPECT_THAT(board.rook_moves(), IsEmpty());
}

TEST_F(BoardTest, InitGameBishopMoves)
{
  auto board = Board::new_board();
  EXPECT_THAT(board.bishop_moves(), IsEmpty());
}

TEST_F(BoardTest, InitGameAllMoves)
{
  MoveVec moves;
  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a3);
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a4);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b3);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b4);
  moves.emplace_back(Piece::pawn(), Sq::c2, Sq::c3);
  moves.emplace_back(Piece::pawn(), Sq::c2, Sq::c4);
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d3);
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d4);
  moves.emplace_back(Piece::pawn(), Sq::e2, Sq::e3);
  moves.emplace_back(Piece::pawn(), Sq::e2, Sq::e4);
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f3);
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f4);
  moves.emplace_back(Piece::pawn(), Sq::g2, Sq::g3);
  moves.emplace_back(Piece::pawn(), Sq::g2, Sq::g4);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h3);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h4);

  // Knight moves
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::a3);
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::c3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::f3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::h3);

  auto board = Board::new_board();
  EXPECT_THAT(board.all_moves(), UnorderedElementsAreArray(moves));
}

TEST_F(BoardTest, WithFen1)
{
  auto board = read_fen("8/1P6/8/1K2n3/6Pp/8/5P2/2k5 w - - 0 1");

  MoveVec moves;

  // Pawn moves with promotion
  moves.emplace_back(Move::promo(Sq::b7, Sq::b8, Piece::knight()));
  moves.emplace_back(Move::promo(Sq::b7, Sq::b8, Piece::bishop()));
  moves.emplace_back(Move::promo(Sq::b7, Sq::b8, Piece::rook()));
  moves.emplace_back(Move::promo(Sq::b7, Sq::b8, Piece::queen()));

  // Regular pawn moves
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f3);
  moves.emplace_back(Piece::pawn(), Sq::f2, Sq::f4);
  moves.emplace_back(Piece::pawn(), Sq::g4, Sq::g5);

  // King moves
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a4);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a5);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a6);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::b4);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::b6);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::c5);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), UnorderedElementsAreArray(moves));
}
