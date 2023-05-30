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

template<Color color, BoardSide side>
constexpr bool
NoneBetweenKingAndRook(BitBoard all_pieces) noexcept
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
  for (; to_squares; to_squares &= to_squares - 1) {
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
    moves.emplace_back(p, from_square, to_piece, to_square);
  }
}

// Computes simples moves for Bishops, Kights, Rooks, and Queens. Simple moves
// consists of non-attack moves and attacks. |piece| is the piece moving,
// |state| is the current board state, and |moves_fn| is a function to compute
// moves for the given piece, including non-attacks and attacks.
void
GetSimpleMoves(
    Piece piece,
    const BoardState& state,
    const std::function<BitBoard(BitBoard)>& moves_fn,
    std::vector<Move>& moves)
{
  auto bb = state.mine[Uint8(piece)];
  auto all_pieces = state.all_mine | state.all_other;
  auto no_pieces = ~all_pieces;

  for (; bb; bb &= bb - 1) {
    auto from_square = std::countr_zero(bb);
    auto bb_piece = 1ull << from_square;

    auto bb_moves = moves_fn(bb_piece);

    // Compute moves to empty squares.
    auto to_squares = bb_moves & no_pieces;
    GetNonAttacks(piece, from_square, to_squares, moves);

    // Compute attacks.
    to_squares = bb_moves & state.all_other;
    GetSimpleAttacks(piece, from_square, to_squares, state.other, moves);
  }
}

// Same as above, but list of moves is returned as output.
std::vector<Move>
GetSimpleMoves(
    Piece piece,
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

  switch (state.next) {
  case Color::White:
    if (state.wk_castle and
        NoneBetweenKingAndRook<Color::White, BoardSide::King>(all_pieces))
      moves.push_back(Move::WhiteKingSideCastle());
    if (state.wq_castle and
        NoneBetweenKingAndRook<Color::White, BoardSide::Queen>(all_pieces))
      moves.push_back(Move::WhiteQueenSideCastle());
    break;
  case Color::Black:
    if (state.bk_castle and
        NoneBetweenKingAndRook<Color::Black, BoardSide::King>(all_pieces))
      moves.push_back(Move::BlackKingSideCastle());
    if (state.bq_castle and
        NoneBetweenKingAndRook<Color::Black, BoardSide::Queen>(all_pieces))
      moves.push_back(Move::BlackQueenSideCastle());
    break;
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
  BitBoard pawn_moves = 0;
  auto no_pieces = ~(state.all_mine | state.all_other);
  BitBoard pawns = state.mine[Uint8(Piece::Pawn)];
  constexpr auto pawn = Uint8(Piece::Pawn);

  std::vector<Move> moves;

  switch (state.next) {
  case Color::White:
    // TODO: need to check if en passant if possible, and if it is, the file
    // where the pawn can be captured.
    pawn_moves = MoveWhitePawnsSingle(pawns, no_pieces);

    for (; pawn_moves; pawn_moves &= pawn_moves - 1) {
      auto to_square = std::countr_zero(pawn_moves);

      if (to_square < 56)
        moves.emplace_back(pawn, to_square - 8, to_square);
      else {
        // Pawn is moving to the last rank for promotion.
        moves.push_back(Move::WhitePawnPromo(to_square, Piece::Queen));
        moves.push_back(Move::WhitePawnPromo(to_square, Piece::Rook));
        moves.push_back(Move::WhitePawnPromo(to_square, Piece::Bishop));
        moves.push_back(Move::WhitePawnPromo(to_square, Piece::Knight));
      }
    }

    pawn_moves = MoveWhitePawnsDouble(pawns, no_pieces);

    for (; pawn_moves; pawn_moves &= pawn_moves - 1) {
      auto to_square = std::countr_zero(pawn_moves);
      auto from_square = to_square - 16;
      moves.emplace_back(pawn, from_square, to_square);
    }

    pawn_moves = MoveWhitePawnsAttackLeft(pawns, state.all_other);

    for (; pawn_moves; pawn_moves &= pawn_moves - 1) {
      auto to_square = std::countr_zero(pawn_moves);
      auto to_bb = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, to_bb);
      auto from_square = to_square - 7;

      if (to_square < 56)
        moves.emplace_back(pawn, from_square, to_piece, to_square);
      else {
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Queen));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Rook));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Bishop));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Knight));
      }
    }

    pawn_moves = MoveWhitePawnsAttackRight(pawns, state.all_other);

    for (; pawn_moves; pawn_moves &= pawn_moves - 1) {
      auto to_square = std::countr_zero(pawn_moves);
      auto to_bb = 1ull << to_square;
      auto to_piece = GetPieceUint8(state.other, to_bb);
      auto from_square = to_square - 9;

      if (to_square < 56)
        moves.emplace_back(pawn, from_square, to_piece, to_square);
      else {
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Queen));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Rook));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Bishop));
        moves.push_back(Move::WhitePawnPromo(
              from_square, to_piece, to_square, Piece::Knight));
      }
    }
  default:
    // TODO: Add logic for black pieces and see about consolidating logic.
    break;
  }

  return moves;
}

std::vector<Move>
MoveGen::AllMoves(const BoardState& state) const
{
  (void)state;
  return std::vector<Move>();
}

} // namespace blunder
