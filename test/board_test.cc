#include "board.h"

#include <iostream>
#include <unordered_set>
#include <vector>

#include "fen.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace blunder;

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

template<typename Collection>
std::string
to_move_list(const Collection& moves)
{
  auto first = moves.begin();
  auto last = moves.end();

  std::string buff;
  buff.reserve(20 * moves.size() + 3);
  buff += '[';

  if (first != last) {
    buff += first->str();
    first++;
  }

  while (first != last) {
    buff += ", ";
    buff += first->str();
    first++;
  }

  buff += ']';

  return buff;
}

MATCHER_P(MovesAre, expected_moves, "the moves are: "
    + to_move_list(expected_moves))
{
  std::unordered_set<Move> actual_mvs(arg.begin(), arg.end());
  std::unordered_set<Move> expected_mvs(
      expected_moves.begin(), expected_moves.end());

  if (actual_mvs == expected_mvs) return true;

  for (auto iter = actual_mvs.begin(); iter != actual_mvs.end();) {
    if (not expected_mvs.contains(*iter))
      ++iter;
    else {
      expected_mvs.erase(*iter);
      iter = actual_mvs.erase(iter);
    }
  }

  if (not expected_mvs.empty()) {
    *result_listener
      << '\n'
      << "ACTUAL result is missing the following entries: "
      << to_move_list(expected_mvs);
  }

  if (not actual_mvs.empty()) {
    *result_listener
      << '\n'
      << "ACTUAL result has the following unexpected entries: "
      << to_move_list(actual_mvs);
  }

  return false;
}

//-----------------------------------------------------------------------------
// https://lichess.org/editor
//
// Note: in tests below, one can use the lichess editor to construct the boards
// from the FEN strings to make it easier to see what the boar and moves are.
//-----------------------------------------------------------------------------

class BoardTest : public testing::Test
{
protected:
  void
  SetUp() override
  { Board::register_magics(); }
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

TEST_F(BoardTest, WithFen2)
{
  auto board = read_fen("8/8/7k/1K6/3b4/R1PP2n1/8/8 w - - 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::c3, Sq::c4);
  moves.emplace_back(Piece::pawn(), Sq::c3, Piece::bishop(), Sq::d4);

  // Rook moves
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a1);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a2);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::b3);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a4);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a5);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a6);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a7);
  moves.emplace_back(Piece::rook(), Sq::a3, Sq::a8);

  // King moves
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a4);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::b4);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::c4);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a5);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::a6);
  moves.emplace_back(Piece::king(), Sq::b5, Sq::c6);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), UnorderedElementsAreArray(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, WithFen3)
{
  auto board = read_fen("2B2b1K/p7/8/n2Q1p2/8/8/3P4/5k2 w - - 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d3);
  moves.emplace_back(Piece::pawn(), Sq::d2, Sq::d4);

  // Bishop moves
  moves.emplace_back(Piece::bishop(), Sq::c8, Sq::b7);
  moves.emplace_back(Piece::bishop(), Sq::c8, Sq::a6);
  moves.emplace_back(Piece::bishop(), Sq::c8, Sq::d7);
  moves.emplace_back(Piece::bishop(), Sq::c8, Sq::e6);
  moves.emplace_back(Piece::bishop(), Sq::c8, Piece::pawn(), Sq::f5);

  // King moves
  moves.emplace_back(Piece::king(), Sq::h8, Sq::g8);
  moves.emplace_back(Piece::king(), Sq::h8, Sq::h7);

  // Queen moves
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::a2);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::b3);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::c4);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::e4);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::f3);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::g2);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::h1);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::d4);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::d3);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::d6);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::d7);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::d8);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::c5);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::b5);
  moves.emplace_back(Piece::queen(), Sq::d5, Piece::knight(), Sq::a5);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::e5);
  moves.emplace_back(Piece::queen(), Sq::d5, Piece::pawn(), Sq::f5);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::g8);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::c6);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::b7);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::a8);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::e6);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::f7);
  moves.emplace_back(Piece::queen(), Sq::d5, Sq::g8);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, WithFen4)
{
  auto board = read_fen(
      "r3k2r/1ppqbppp/p1np1n2/4p3/2B1PPb1/2NP1N2/PPPBQ1PP/R3K2R w KQkq - 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a3);
  moves.emplace_back(Piece::pawn(), Sq::a2, Sq::a4);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b3);
  moves.emplace_back(Piece::pawn(), Sq::b2, Sq::b4);
  moves.emplace_back(Piece::pawn(), Sq::d3, Sq::d4);
  moves.emplace_back(Piece::pawn(), Sq::f4, Sq::f5);
  moves.emplace_back(Piece::pawn(), Sq::f4, Piece::pawn(), Sq::e5);
  moves.emplace_back(Piece::pawn(), Sq::g2, Sq::g3);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h3);
  moves.emplace_back(Piece::pawn(), Sq::h2, Sq::h4);

  // Knight moves
  moves.emplace_back(Piece::knight(), Sq::c3, Sq::b1);
  moves.emplace_back(Piece::knight(), Sq::c3, Sq::a4);
  moves.emplace_back(Piece::knight(), Sq::c3, Sq::b5);
  moves.emplace_back(Piece::knight(), Sq::c3, Sq::d5);
  moves.emplace_back(Piece::knight(), Sq::c3, Sq::d1);
  moves.emplace_back(Piece::knight(), Sq::f3, Sq::d4);
  moves.emplace_back(Piece::knight(), Sq::f3, Piece::pawn(), Sq::e5);
  moves.emplace_back(Piece::knight(), Sq::f3, Sq::g5);
  moves.emplace_back(Piece::knight(), Sq::f3, Sq::h4);
  moves.emplace_back(Piece::knight(), Sq::f3, Sq::g1);

  // Bishop moves
  moves.emplace_back(Piece::bishop(), Sq::c4, Sq::b3);
  moves.emplace_back(Piece::bishop(), Sq::c4, Sq::b5);
  moves.emplace_back(Piece::bishop(), Sq::c4, Piece::pawn(), Sq::a6);
  moves.emplace_back(Piece::bishop(), Sq::c4, Sq::d5);
  moves.emplace_back(Piece::bishop(), Sq::c4, Sq::e6);
  moves.emplace_back(Piece::bishop(), Sq::c4, Piece::pawn(), Sq::f7);
  moves.emplace_back(Piece::bishop(), Sq::d2, Sq::c1);
  moves.emplace_back(Piece::bishop(), Sq::d2, Sq::e3);

  // Rook moves
  moves.emplace_back(Piece::rook(), Sq::a1, Sq::b1);
  moves.emplace_back(Piece::rook(), Sq::a1, Sq::c1);
  moves.emplace_back(Piece::rook(), Sq::a1, Sq::d1);
  moves.emplace_back(Piece::rook(), Sq::h1, Sq::f1);
  moves.emplace_back(Piece::rook(), Sq::h1, Sq::g1);

  // Queen moves
  moves.emplace_back(Piece::queen(), Sq::e2, Sq::d1);
  moves.emplace_back(Piece::queen(), Sq::e2, Sq::e3);
  moves.emplace_back(Piece::queen(), Sq::e2, Sq::f2);
  moves.emplace_back(Piece::queen(), Sq::e2, Sq::f1);

  // King moves
  moves.emplace_back(Piece::king(), Sq::e1, Sq::d1);
  moves.emplace_back(Piece::king(), Sq::e1, Sq::f1);
  moves.emplace_back(Piece::king(), Sq::e1, Sq::f2);
  moves.emplace_back(Move::wk_castle());
  moves.emplace_back(Move::wq_castle());

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}


TEST_F(BoardTest, WithFen5)
{
  auto board = read_fen(
      "r3k2r/1ppqbppp/p1np1n2/4p3/2B1PPb1/2NP1N2/PPPBQ1PP/2KR3R b Kkq - 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::a6, Sq::a5);
  moves.emplace_back(Piece::pawn(), Sq::b7, Sq::b6);
  moves.emplace_back(Piece::pawn(), Sq::b7, Sq::b5);
  moves.emplace_back(Piece::pawn(), Sq::d6, Sq::d5);
  moves.emplace_back(Piece::pawn(), Sq::e5, Piece::pawn(), Sq::f4);
  moves.emplace_back(Piece::pawn(), Sq::g7, Sq::g6);
  moves.emplace_back(Piece::pawn(), Sq::g7, Sq::g5);
  moves.emplace_back(Piece::pawn(), Sq::h7, Sq::h6);
  moves.emplace_back(Piece::pawn(), Sq::h7, Sq::h5);

  // Knight moves
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::d8);
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::b8);
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::a7);
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::a5);
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::b4);
  moves.emplace_back(Piece::knight(), Sq::c6, Sq::d4);
  moves.emplace_back(Piece::knight(), Sq::f6, Sq::d5);
  moves.emplace_back(Piece::knight(), Sq::f6, Piece::pawn(), Sq::e4);
  moves.emplace_back(Piece::knight(), Sq::f6, Sq::h5);
  moves.emplace_back(Piece::knight(), Sq::f6, Sq::g8);

  // Bishop moves
  moves.emplace_back(Piece::bishop(), Sq::e7, Sq::d8);
  moves.emplace_back(Piece::bishop(), Sq::e7, Sq::f8);
  moves.emplace_back(Piece::bishop(), Sq::g4, Piece::knight(), Sq::f3);
  moves.emplace_back(Piece::bishop(), Sq::g4, Sq::h3);
  moves.emplace_back(Piece::bishop(), Sq::g4, Sq::h5);
  moves.emplace_back(Piece::bishop(), Sq::g4, Sq::f5);
  moves.emplace_back(Piece::bishop(), Sq::g4, Sq::e6);

  // Rook moves
  moves.emplace_back(Piece::rook(), Sq::a8, Sq::a7);
  moves.emplace_back(Piece::rook(), Sq::a8, Sq::b8);
  moves.emplace_back(Piece::rook(), Sq::a8, Sq::c8);
  moves.emplace_back(Piece::rook(), Sq::a8, Sq::d8);
  moves.emplace_back(Piece::rook(), Sq::h8, Sq::f8);
  moves.emplace_back(Piece::rook(), Sq::h8, Sq::g8);

  // Queen moves
  moves.emplace_back(Piece::queen(), Sq::d7, Sq::c8);
  moves.emplace_back(Piece::queen(), Sq::d7, Sq::d8);
  moves.emplace_back(Piece::queen(), Sq::d7, Sq::e6);
  moves.emplace_back(Piece::queen(), Sq::d7, Sq::f5);

  // King moves
  moves.emplace_back(Piece::king(), Sq::e8, Sq::d8);
  moves.emplace_back(Piece::king(), Sq::e8, Sq::f8);
  moves.emplace_back(Move::bk_castle());
  moves.emplace_back(Move::bq_castle());

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, WithFen6)
{
  auto board = read_fen("3k4/2p5/8/2Pp4/8/8/4K3/8 w - d6 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::c5, Sq::c6);
  moves.emplace_back(Move::by_enpassant(Sq::c5, Sq::d6, Sq::d5));

  // King moves
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::e1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d2);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f2);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d3);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::e3);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f3);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}


TEST_F(BoardTest, WithFen7)
{
  auto board = read_fen("3k4/2p5/8/1pP5/8/8/4K3/8 w - b6 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::c5, Sq::c6);
  moves.emplace_back(Move::by_enpassant(Sq::c5, Sq::b6, Sq::b5));

  // King moves
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::e1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f1);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d2);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f2);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::d3);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::e3);
  moves.emplace_back(Piece::king(), Sq::e2, Sq::f3);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, WithFen8)
{
  auto board = read_fen("8/8/8/3k4/2pP4/8/2P5/2K5 b - d3 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::c4, Sq::c3);
  moves.emplace_back(Move::by_enpassant(Sq::c4, Sq::d3, Sq::d4));

  // King moves
  moves.emplace_back(Piece::king(), Sq::d5, Sq::c6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::d6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::e6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::e4);
  moves.emplace_back(Piece::king(), Sq::d5, Piece::pawn(), Sq::d4);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, WithFen9)
{
  auto board = read_fen("8/8/8/3k4/3Pp3/8/2P5/2K5 b - d3 0 1");

  MoveVec moves;

  // Pawn moves
  moves.emplace_back(Piece::pawn(), Sq::e4, Sq::e3);
  moves.emplace_back(Move::by_enpassant(Sq::e4, Sq::d3, Sq::d4));

  // King moves
  moves.emplace_back(Piece::king(), Sq::d5, Sq::c6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::d6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::e6);
  moves.emplace_back(Piece::king(), Sq::d5, Sq::c4);
  moves.emplace_back(Piece::king(), Sq::d5, Piece::pawn(), Sq::d4);

  ASSERT_TRUE(board);
  EXPECT_THAT(board->all_moves(), MovesAre(moves))
    << '\n' << *board;
}

TEST_F(BoardTest, TwoUpdatesFromBeginning)
{
  auto board = Board::new_board();

  EXPECT_TRUE(board.is_white_next());

  Move e4(Piece::pawn(), Sq::e2, Sq::e4);
  EXPECT_TRUE(board.update_with_move(e4));

  EXPECT_FALSE(board.is_white_next());

  Move e5(Piece::pawn(), Sq::e7, Sq::e5);
  EXPECT_TRUE(board.update_with_move(e5));

  EXPECT_TRUE(board.is_white_next());

  // TODO: Add more testing around update logic.
}
