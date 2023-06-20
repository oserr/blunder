#include "move_gen.h"

#include <vector>

#include "board_state.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "magic.h"

using namespace blunder;

using testing::UnorderedElementsAreArray;

class MoveGenTest : public testing::Test
{
protected:
  // TODO: initialize MoveGen with mock MagicAttacks.
  MoveGenTest()
    : move_gen_(
        MagicAttacks(std::vector<Magic>()),
        MagicAttacks(std::vector<Magic>())) {}

  MoveGen move_gen_;
};

TEST_F(MoveGenTest, InitGamePawnMoves)
{
  std::vector<Move> moves;
  moves.emplace_back(Piece::Pawn, Sq::a2, Sq::a3);
  moves.emplace_back(Piece::Pawn, Sq::a2, Sq::a4);
  moves.emplace_back(Piece::Pawn, Sq::b2, Sq::b3);
  moves.emplace_back(Piece::Pawn, Sq::b2, Sq::b4);
  moves.emplace_back(Piece::Pawn, Sq::c2, Sq::c3);
  moves.emplace_back(Piece::Pawn, Sq::c2, Sq::c4);
  moves.emplace_back(Piece::Pawn, Sq::d2, Sq::d3);
  moves.emplace_back(Piece::Pawn, Sq::d2, Sq::d4);
  moves.emplace_back(Piece::Pawn, Sq::e2, Sq::e3);
  moves.emplace_back(Piece::Pawn, Sq::e2, Sq::e4);
  moves.emplace_back(Piece::Pawn, Sq::f2, Sq::f3);
  moves.emplace_back(Piece::Pawn, Sq::f2, Sq::f4);
  moves.emplace_back(Piece::Pawn, Sq::g2, Sq::g3);
  moves.emplace_back(Piece::Pawn, Sq::g2, Sq::g4);
  moves.emplace_back(Piece::Pawn, Sq::h2, Sq::h3);
  moves.emplace_back(Piece::Pawn, Sq::h2, Sq::h4);

  auto state = NewBoardState();
  EXPECT_THAT(move_gen_.PawnMoves(state), UnorderedElementsAreArray(moves));
}
