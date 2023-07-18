#include "move_gen.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>

#include "bitboard.h"
#include "board_state.h"
#include "color.h"
#include "move.h"
#include "moves.h"
#include "pieces.h"

namespace blunder {
namespace {

// Returns all the non-attack moves for |piece| from square |from_square| to all
// squares in |to_squares|. The moves are returned in |moves|, an output
// parameter.
inline void
get_non_attacks(
    Piece piece,
    std::uint8_t from_square,
    BitBoard to_squares,
    MoveVec& moves)
{
  while (to_squares)
    moves.emplace_back(piece, from_square, to_squares.first_bit_and_clear());
}

// Returns all the simple attack moves for |piece| from square |from_square| to
// squares in |to_squares|. We pass in the |other| pieces to be able to figure
// out what the attacked pieces are.
void
get_simple_attacks(
    Piece piece,
    std::uint8_t from_square,
    BitBoard to_squares,
    const PieceSet& other,
    MoveVec& moves)
{
  while (to_squares) {
    auto [to_square, attacked] = to_squares.index_bb_and_clear();
    auto to_piece = other.find_type(attacked);
    assert(to_piece.type() != Type::None);
    moves.emplace_back(piece, from_square, to_piece, to_square);
  }
}

// Computes simples moves for Bishops, Kights, Rooks, and Queens. Simple moves
// consists of non-attack moves and attacks. |piece| is the piece moving,
// |state| is the current board state, and |moves_fn| is a function to compute
// moves for the given piece, including non-attacks and attacks.
void
get_simple_moves(
    Piece piece,
    const Board& state,
    const std::function<BitBoard(BitBoard)>& moves_fn,
    MoveVec& moves)
{
  auto bb = state.mine().get(piece);
  auto no_pieces = state.none();

  while (bb) {
    auto [from_square, bb_piece] = bb.index_bb_and_clear();
    auto bb_moves = moves_fn(bb_piece);

    // Compute moves to empty squares.
    auto to_squares = bb_moves & no_pieces;
    get_non_attacks(piece, from_square, to_squares, moves);

    // Compute attacks.
    to_squares = bb_moves & state.all_other();
    get_simple_attacks(piece, from_square, to_squares, state.other(), moves);
  }
}

// Same as above, but list of moves is returned as output.
MoveVec
get_simple_moves(
    Piece piece,
    const Board& state,
    const std::function<BitBoard(BitBoard)>& moves_fn)
{
  MoveVec moves;
  get_simple_moves(piece, state, moves_fn, moves);
  return moves;
}

//----------------------------------------
// Helper functions to compute pawn moves.
//----------------------------------------

using PawnMovesFn = BitBoard(*)(BitBoard, BitBoard);
using FromFn = std::uint8_t(*)(std::uint8_t);
using IsPromoFn = bool(*)(std::uint8_t);

// Used to check if the move represents a promotion for white.
bool
is_white_promo(std::uint8_t to_square) noexcept
{ return to_square >= 56; }

// Used to check if the move represents a promotion for black.
bool
is_black_promo(std::uint8_t to_square) noexcept
{ return to_square <= 7; }

// Computes the from square for a one-square white pawn move given the
// destination square.
std::uint8_t
from_single_white(std::uint8_t to_square) noexcept
{ return to_square - 8; }

// Computes the from square for a double-square white pawn move given the
// destination square.
std::uint8_t
from_double_white(std::uint8_t to_square) noexcept
{ return to_square - 16; }

// Computes the from square for a white pawn attack diagonal left.
std::uint8_t
from_left_white(std::uint8_t to_square) noexcept
{ return to_square - 7; }

// Computes the from square for a white pawn attack diagonal right.
std::uint8_t
from_right_white(std::uint8_t to_square) noexcept
{ return to_square - 9; }

// Computes the from square for a one-square black pawn move given the
// destination square.
std::uint8_t
from_single_black(std::uint8_t to_square) noexcept
{ return to_square + 8; }

// Computes the from square for a double-square black pawn move given the
// destination square.
std::uint8_t
from_double_black(std::uint8_t to_square) noexcept
{ return to_square + 16; }

// Computes the from square for a black pawn attack diagonal left.
std::uint8_t
from_left_black(std::uint8_t to_square) noexcept
{ return to_square + 7; }

// Computes the from square for a black pawn attack diagonal right.
std::uint8_t
from_right_black(std::uint8_t to_square) noexcept
{ return to_square + 9; }

// Function to generate forward pawn moves.
void
move_forward(
    BitBoard pawns,
    BitBoard no_pieces,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    MoveVec& moves)
{
  auto pawn_moves = move_fn(pawns, no_pieces);

  while (pawn_moves) {
    auto to_square = pawn_moves.first_bit_and_clear();
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_square);
    else {
      // Pawn is moving to the last rank for promotion.
      moves.push_back(Move::promo(from_square, to_square, Piece::queen()));
      moves.push_back(Move::promo(from_square, to_square, Piece::rook()));
      moves.push_back(Move::promo(from_square, to_square, Piece::bishop()));
      moves.push_back(Move::promo(from_square, to_square, Piece::knight()));
    }
  }
}

// Function to generate pawn attacks.
void
attack(
    BitBoard pawns,
    Board state,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    MoveVec& moves)
{
  auto pawn_moves = move_fn(pawns, state.all_other());

  while (pawn_moves) {
    auto [to_square, to_bb] = pawn_moves.index_bb_and_clear();
    auto to_piece = state.other().find_type(to_bb);
    assert(to_piece.type() != Type::None);
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_piece, to_square);
    else {
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::queen()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::rook()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::bishop()));
      moves.push_back(Move::promo(
            from_square, to_piece, to_square, Piece::knight()));
    }
  }
}

// We create a BitBoard with one bit set where the pawn moves after capturing
// en passant, and use that to check if there are any captures.
void
move_enpassant(
    const Board& state,
    MoveVec& moves)
{
  if (not state.has_enpassant())
    return;

  // Fake square
  auto pawns = state.mine().pawn();
  unsigned to_sq = state.enpassant_file();
  unsigned passant_sq = to_sq;

  if (state.is_white_next()) {
    to_sq += 40; // 6th row
    passant_sq += 32; // 5th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = move_wp_left(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_white(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = move_wp_right(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_white(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  } else {
    to_sq += 16; // 3rd row
    passant_sq += 24; // 4th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = move_bp_left(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_black(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = move_bp_right(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_black(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  }
}

} // namespace

MoveVec
MoveGen::for_king(const Board& state) const
{
  MoveVec moves;
  for_king(state, moves);
  return moves;
}

// Generates all possible moves for queens on the board.
MoveVec
MoveGen::for_queen(const Board& state) const
{
  MoveVec moves;
  for_queen(state, moves);
  return moves;
}

MoveVec
MoveGen::for_rook(const Board& state) const
{
  MoveVec moves;
  for_rook(state, moves);
  return moves;
}

MoveVec
MoveGen::for_bishop(const Board& state) const
{
  MoveVec moves;
  for_bishop(state, moves);
  return moves;
}

MoveVec
MoveGen::for_knight(const Board& state) const
{
  return get_simple_moves(Piece::knight(), state, move_knight);
}

MoveVec
MoveGen::for_pawn(const Board& state) const
{
  MoveVec moves;
  for_pawn(state, moves);
  return moves;
}

MoveVec
MoveGen::for_all(const Board& state) const
{
  // TODO: determine if there are any performance gains from computing the moves
  // in parallel.
  MoveVec moves;
  for_king(state, moves);
  for_queen(state, moves);
  for_rook(state, moves);
  for_bishop(state, moves);
  for_knight(state, moves);
  for_pawn(state, moves);
  return moves;
}

void
MoveGen::for_king(
    const Board& state,
    MoveVec& moves) const
{
  get_simple_moves(Piece::king(), state, move_king, moves);

  if (state.is_white_next()) {
    if (state.wk_can_castle())
      moves.push_back(Move::wk_castle());
    if (state.wq_can_castle())
      moves.push_back(Move::wq_castle());
  } else {
    if (state.bk_can_castle())
      moves.push_back(Move::bk_castle());
    if (state.bq_can_castle())
      moves.push_back(Move::wq_castle());
  }
}

void
MoveGen::for_queen(
    const Board& state,
    MoveVec& moves) const
{
  auto all_pieces = state.all_bits();
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics.get_attacks(from_square, all_pieces)
         | bmagics.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::queen(), state, moves_fn, moves);
}

void
MoveGen::for_rook(
    const Board& state,
    MoveVec& moves) const
{
  auto all_pieces = state.all_bits();
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::rook(), state, moves_fn, moves);
}

void
MoveGen::for_bishop(
    const Board& state,
    MoveVec& moves) const
{
  auto all_pieces = state.all_bits();
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return bmagics.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::bishop(), state, moves_fn, moves);
}

void
MoveGen::for_knight(
    const Board& state,
    MoveVec& moves) const
{
  get_simple_moves(Piece::knight(), state, move_knight, moves);
}

void
MoveGen::for_pawn(
    const Board& state,
    MoveVec& moves) const
{
  BitBoard pawns = state.mine().pawn();

  if (not pawns) return;

  PawnMovesFn single_fn;
  PawnMovesFn double_fn;
  PawnMovesFn attack_left_fn;
  PawnMovesFn attack_right_fn;
  FromFn from_single_fn;
  FromFn from_double_fn;
  FromFn from_left_fn;
  FromFn from_right_fn;
  IsPromoFn is_promo_fn;

  if (state.is_white_next()) {
    single_fn = move_wp_single;
    double_fn = move_wp_double;
    attack_left_fn = move_wp_left;
    attack_right_fn = move_wp_right;
    from_single_fn = from_single_white;
    from_double_fn = from_double_white;
    from_left_fn = from_left_white;
    from_right_fn = from_right_white;
    is_promo_fn = is_white_promo;
  } else {
    single_fn = move_bp_single;
    double_fn = move_bp_double;
    attack_left_fn = move_bp_left;
    attack_right_fn = move_bp_right;
    from_single_fn = from_single_black;
    from_double_fn = from_double_black;
    from_left_fn = from_left_black;
    from_right_fn = from_right_black;
    is_promo_fn = is_black_promo;
  }

  auto no_pieces = ~state.all_bits();

  // TODO: handle en passant moves
  move_forward(pawns, no_pieces, single_fn, from_single_fn, is_promo_fn, moves);
  move_forward(pawns, no_pieces, double_fn, from_double_fn, is_promo_fn, moves);
  attack(pawns, state, attack_left_fn, from_left_fn, is_promo_fn, moves);
  attack(pawns, state, attack_right_fn, from_right_fn, is_promo_fn, moves);
  move_enpassant(state, moves);
}

} // namespace blunder
