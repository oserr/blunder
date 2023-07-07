#include "move_gen.h"

#include <memory>
#include <vector>

#include "board_state.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pre_computed_magics.h"
#include "magic.h"

using namespace blunder;

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

// Only init MoveGen once because it's expensive to init the magics.
constinit std::unique_ptr<MoveGen> move_gen = nullptr;

class MoveGenTest : public testing::Test
{
protected:
  void
  SetUp() override
  {
    if (not move_gen) {
      auto bmagics = from_bmagics(kBishopMagics);
      auto rmagics = from_rmagics(kRookMagics);
      ASSERT_TRUE(bmagics) << "Unable to init bishop magics for MoveGen.";
      ASSERT_TRUE(rmagics) << "Unable to init rook magics for MoveGen.";
      move_gen = std::make_unique<MoveGen>(
          std::move(*bmagics), std::move(*rmagics));
    }
  }
};

TEST_F(MoveGenTest, InitGamePawnMoves)
{
  std::vector<Move> moves;
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

  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_pawn(state), UnorderedElementsAreArray(moves));
}

TEST_F(MoveGenTest, InitGameKingMoves)
{
  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_king(state), IsEmpty());
}

TEST_F(MoveGenTest, InitGameKnightMoves)
{
  std::vector<Move> moves;
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::a3);
  moves.emplace_back(Piece::knight(), Sq::b1, Sq::c3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::f3);
  moves.emplace_back(Piece::knight(), Sq::g1, Sq::h3);

  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_knight(state), UnorderedElementsAreArray(moves));
}

TEST_F(MoveGenTest, InitGameQueenMoves)
{
  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_queen(state), IsEmpty());
}

TEST_F(MoveGenTest, InitGameRookMoves)
{
  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_rook(state), IsEmpty());
}

TEST_F(MoveGenTest, InitGameBishopMoves)
{
  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_bishop(state), IsEmpty());
}

TEST_F(MoveGenTest, InitGameAllMoves)
{
  std::vector<Move> moves;
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

  auto state = NewBoardState();
  EXPECT_THAT(move_gen->for_all(state), UnorderedElementsAreArray(moves));
}
