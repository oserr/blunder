#include "move_gen.h"

#include <vector>

#include "board_state.h"
#include "move.h"
#include "moves.h"
#include "pieces.h"

namespace blunder {

std::vector<Move>
MoveGen::KingMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

// Generates all possible moves for queens on the board.
std::vector<Move>
MoveGen::QueenMoves(const BoardState& state) const
{
  constexpr auto piece = Uint8(Piece::Queen);
  auto queens = state.mine[piece];
  auto all_pieces = state.all_mine | state.all_other;
  BitBoard no_pieces = ~all_pieces;

  std::vector<Move> moves;

  while (queens) {
    auto from_square = std::countr_zero(queens);
    BitBoard queen = 1ull << from_square;

    // TODO: figure out how to include the squares at the outer ranks and files,
    // which are ignored by magic bitboards.
    auto queen_moves = rmagics_.GetAttacks(queen, state.all_pieces);
    queen_moves |= bmagics_.GetAttacks(queen, state.all_pieces);

    // Compute moves to empty squares.
    auto to_squares = no_pieces & queen_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      moves.emplace_back(piece, from_square, to_square);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Compute attacks.
    to_squares = state.all_other & queen_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      BitBoard attacked = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, attacked);
      assert(to_piece >= 0 and to_piece < 6);
      moves.emplace_back(piece, from_square, to_square, to_piece);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Clear the queen we processed.
    queens &= queens - 1;
  }

  return moves;
}

}

std::vector<Move>
MoveGen::RookMoves(const BoardState& state) const
{
  constexpr auto piece = Uint8(Piece::Rook);
  auto rooks = state.mine[piece];
  auto all_pieces = state.all_mine | state.all_other;
  BitBoard no_pieces = ~all_pieces;

  std::vector<Move> moves;

  while (rooks) {
    auto from_square = std::countr_zero(bishops);
    BitBoard rook = 1ull << from_square;

    // TODO: figure out how to include the squares at the outer ranks and files,
    // which are ignored by magic bitboards.
    auto rook_moves = rmagics_.GetAttacks(rook, state.all_pieces);

    // Compute moves to empty squares.
    auto to_squares = no_pieces & rook_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      moves.emplace_back(piece, from_square, to_square);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Compute attacks.
    to_squares = state.all_other & rook_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      BitBoard attacked = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, attacked);
      assert(to_piece >= 0 and to_piece < 6);
      moves.emplace_back(piece, from_square, to_square, to_piece);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Clear the rook we processed.
    rooks &= rooks - 1;
  }

  return moves;
}

std::vector<Move>
MoveGen::BishopMoves(const BoardState& state) const
{
  constexpr auto piece = Uint8(Piece::Bishop);
  auto bishops = state.mine[piece];
  auto all_pieces = state.all_mine | state.all_other;
  BitBoard no_pieces = ~all_pieces;

  std::vector<Move> moves;

  while (bishops) {
    auto from_square = std::countr_zero(bishops);
    BitBoard bishop = 1ull << from_square;

    // TODO: figure out how to include the squares at the outer ranks and files,
    // which are ignored by magic bitboards.
    auto bishop_moves = bmagics_.GetAttacks(bishop, state.all_pieces);

    // Compute moves to empty squares.
    auto to_squares = no_pieces & bishop_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      moves.emplace_back(piece, from_square, to_square);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Compute attacks.
    to_squares = state.all_other & bishop_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      BitBoard attacked = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, attacked);
      assert(to_piece >= 0 and to_piece < 6);
      moves.emplace_back(piece, from_square, to_square, to_piece);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Clear the bishop we processed.
    bishops &= bishops - 1;
  }

  return moves;
}

std::vector<Move>
MoveGen::KnightMoves(const BoardState& state) const
{
  constexpr auto piece = Uint8(Piece::Knight);
  auto knights = state.mine[piece];
  BitBoard no_pieces = ~(state.all_mine | state.all_other);

  std::vector<Move> moves;

  while (knights) {
    auto from_square = std::countr_zero(knights);
    BitBoard knigth = 1ull << from_square;

    auto knight_moves = MoveKnight(knight);

    // Compute moves to empty squares.
    auto to_squares = no_pieces & knight_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      moves.emplace_back(piece, from_square, to_square);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Compute attacks.
    to_squares = state.all_other & knight_moves;
    while (to_squares) {
      auto to_square = std::countr_zero(to_squares);
      BitBoard attacked = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, attacked);
      assert(to_piece >= 0 and to_piece < 6);
      moves.emplace_back(piece, from_square, to_square, to_piece);
      // Clear the square we processed.
      to_squares &= to_squares - 1;
    }

    // Clear the knight we processed.
    knights &= knights - 1;
  }

  return moves;
}

std::vector<Move>
MoveGen::PawnMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

std::vector<Move>
MoveGen::AllMoves(const BoardState& state) const
{
  return std::vector<Move>();
}

} // namespace blunder
