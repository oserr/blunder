#include "move_gen.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <vector>

#include "bitboard.h"
#include "board_side.h"
#include "board_state.h"
#include "color.h"
#include "move.h"
#include "moves.h"
#include "pieces.h"

namespace blunder {
namespace {

constexpr bool
NoneBetweenKingAndRook(BitBoard all_pieces, Color color, BoardSide side)
noexcept
{
  std::uint8_t bits = 0;
  std::uint8_t mask = 0;

  if constexpr (side == BoardSide::King) {
    bits = 0b00001001ull;
    mask = 0b00001111ull;
  } else {
    bits = 0b10001000ull;
    mask = 0b11111000ull;
  }

  if constexpr (color == Color::Black)
    all_pieces >>= 56;

  all_pieces &= 0xffull;
  all_pieces &= mask;

  return bits == all_pieces;
}

// Returns all the non-attack moves for |piece| from square |from_square| to all
// squares in |to_squares|. The moves are returned in |moves|, an output
// parameter.
void
GetNonAttacks(
    Piece piece,
    std::uint8_t from_square,
    std::uint8_t to_squares,
    std::vector<Move>& moves)
{
  auto p = Uint8(piece);
  for (; to_squares; to_square &= to_squares - 1) {
    auto to_square = std::countr_zero(to_squares);
    moves.emplace_back(p, from_square, to_square);
  }
}

// Returns all the simple attack moves for |piece| from square |from_square| to
// squares in |to_squares|. We pass in the |other| pieces to be able to figure
// out what the attacked pieces are.
void
GetSimpleAttacks(
    Piece piece,
    std::uint8_t from_square,
    std::uint8_t to_squares,
    const PieceSet other,
    std::vector<Move>& moves)
{
  auto p = Uint8(piece);
  for (; to_squares; to_squares &= to_squares - 1) {
    auto to_square = std::countr_zero(to_squares);
    BitBoard attacked = 1ull << to_square;
    auto to_piece = GetPieceUint8(other, attacked);
    assert(to_piece >= 0 and to_piece < 6);
    moves.emplace_back(p, from_square, to_square, to_piece);
  }
}

// Computes simples moves for Bishops, Kights, Rooks, and Queens. Simple moves
// consists of non-attack moves and attacks. |piece| is the piece moving,
// |state| is the current board state, and |moves_fn| is a function to compute
// moves for the given piece, including non-attacks and attacks.
void
GetSimpleMoves(
    Piece piece
    const BoardState& state,
    const std::function<BitBoard(BitBoard)>& moves_fn,
    std::vector<Move>& moves)
{
  auto bb = state.mine[Uint8(piece)];
  auto all_pieces = state.all_mine | state.all_other;
  auto no_pieces = ~all_pieces;

  std::vector<Move> moves;

  for (; bb; bb &= bb - 1) {
    auto from_square = std::countr_zero(bb);
    auto bb_piece = 1ull << from_square;

    auto bb_moves = move_fn(bb_piece);

    // Compute moves to empty squares.
    auto to_squares = bb_moves & no_pieces;
    GetNonAttacks(piece, from_square, to_squares, moves);

    // Compute attacks.
    to_squares = bb_moves & state.all_other;
    GetSimpleAttackes(piece, from_square, to_squares, state.other, moves);
  }
}

// Same as above, but list of moves is returned as output.
std::vector<Move>
GetSimpleMoves(
    Piece piece
    const BoardState& state,
    const std::function<BitBoard(BitBoard)>& moves_fn)
{
  std::vector<Move> moves;
  GetSimpleMoves(piece, state, moves_fn, moves);
  return moves;
}
} // namespace

std::vector<Move>
MoveGen::KingMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  GetSimpleMoves(Piece::King, state, MoveKing, moves);

  auto all_pieces = state.all_mine | state.all_other;
  constexpr auto p = Uint8(Piece::King);

  if (state.next == Color::White and state.wk_castle) {
    if (NoneBetweenKingAndRook(all_pieces, Color::White, BoardSide::King))
      moves.emplace(Move::WhiteKingSideCastle());
    if (NoneBetweenKingAndRook(all_pieces, Color::White, BoardSide::Queen))
      moves.emplace(Move::WhiteQueenSideCastle());
  } else if (state.next == Color::Black && state.bk_castle) {
    if (NoneBetweenKingAndRook(all_pieces, Color::Black, BoardSide::King))
      moves.emplace(Move::BlackKingSideCastle());
    if (NoneBetweenKingAndRook(all_pieces, Color::Black, BoardSide::Queen))
      moves.emplace(Move::BlackQueenSideCastle());
  }

  return moves;
}

// Generates all possible moves for queens on the board.
std::vector<Move>
MoveGen::QueenMoves(const BoardState& state) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    return rmagics_.GetAttacks(bb, all_pieces)
         | bmagics_.GetAttacks(bb, all_pieces);
  };
  return GetSimpleMoves(Piece::Queen, state, moves_fn);
}

std::vector<Move>
MoveGen::RookMoves(const BoardState& state) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    return rmagics_.GetAttacks(bb, all_pieces);
  };
  return GetSimpleMoves(Piece::Rook, state, moves_fn);
}

std::vector<Move>
MoveGen::BishopMoves(const BoardState& state) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    return bmagics_.GetAttacks(bb, all_pieces);
  };
  return GetSimpleMoves(Piece::Bishop, state, moves_fn);
}

std::vector<Move>
MoveGen::KnightMoves(const BoardState& state) const
{
  return GetSimpleMoves(Piece::Knight, state, MoveKnight);
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
