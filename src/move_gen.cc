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

// TODO: make this a function of BoardState.
template<Color color, BoardSide side>
constexpr bool
can_castle(BitBoard all_pieces) noexcept
{
  BitBoard bits;
  BitBoard mask;

  if constexpr (side == BoardSide::King) {
    bits.set_bits(0b00001001ull);
    mask.set_bits(0b00001111ull);
  } else {
    bits.set_bits(0b10001000ull);
    mask.set_bits(0b11111000ull);
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
inline void
get_non_attacks(
    Piece piece,
    std::uint8_t from_square,
    BitBoard to_squares,
    std::vector<Move>& moves)
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
    const PieceSet other,
    std::vector<Move>& moves)
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
    const BoardState& state,
    const std::function<BitBoard(BitBoard)>& moves_fn,
    std::vector<Move>& moves)
{
  auto bb = state.mine.get(piece);
  auto all_pieces = state.all_mine | state.all_other;
  auto no_pieces = ~all_pieces;

  while (bb) {
    auto [from_square, bb_piece] = bb.index_bb_and_clear();
    auto bb_moves = moves_fn(bb_piece);

    // Compute moves to empty squares.
    auto to_squares = bb_moves & no_pieces;
    get_non_attacks(piece, from_square, to_squares, moves);

    // Compute attacks.
    to_squares = bb_moves & state.all_other;
    get_simple_attacks(piece, from_square, to_squares, state.other, moves);
  }
}

// Same as above, but list of moves is returned as output.
std::vector<Move>
get_simple_moves(
    Piece piece,
    const BoardState& state,
    const std::function<BitBoard(BitBoard)>& moves_fn)
{
  std::vector<Move> moves;
  get_simple_moves(piece, state, moves_fn, moves);
  return moves;
}

using PawnMovesFn = BitBoard(*)(BitBoard, BitBoard);
using FromFn = std::uint8_t(*)(std::uint8_t);
using IsPromoFn = bool(*)(std::uint8_t);

bool
is_white_promo(std::uint8_t to_square) noexcept
{ return to_square >= 56; }

bool
is_black_promo(std::uint8_t to_square) noexcept
{ return to_square <= 7; }

std::uint8_t
from_single_white(std::uint8_t to_square) noexcept
{ return to_square - 8; }

std::uint8_t
from_double_white(std::uint8_t to_square) noexcept
{ return to_square - 16; }

std::uint8_t
from_left_white(std::uint8_t to_square) noexcept
{ return to_square - 7; }

std::uint8_t
from_right_white(std::uint8_t to_square) noexcept
{ return to_square - 9; }

std::uint8_t
from_single_black(std::uint8_t to_square) noexcept
{ return to_square + 8; }

std::uint8_t
from_double_black(std::uint8_t to_square) noexcept
{ return to_square + 16; }

std::uint8_t
from_left_black(std::uint8_t to_square) noexcept
{ return to_square + 7; }

std::uint8_t
from_right_black(std::uint8_t to_square) noexcept
{ return to_square + 9; }

void
move_forward(
    BitBoard pawns,
    BitBoard no_pieces,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    std::vector<Move>& moves)
{
  auto pawn_moves = move_fn(pawns, no_pieces);

  while (pawn_moves) {
    auto to_square = pawn_moves.first_bit_and_clear();
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_square);
    else {
      // Pawn is moving to the last rank for promotion.
      moves.push_back(Move::PawnPromo(from_square, to_square, Piece::queen()));
      moves.push_back(Move::PawnPromo(from_square, to_square, Piece::rook()));
      moves.push_back(Move::PawnPromo(from_square, to_square, Piece::bishop()));
      moves.push_back(Move::PawnPromo(from_square, to_square, Piece::knight()));
    }
  }
}

void
attack(
    BitBoard pawns,
    BoardState state,
    PawnMovesFn move_fn,
    FromFn from_fn,
    IsPromoFn is_promo_fn,
    std::vector<Move>& moves)
{
  auto pawn_moves = move_fn(pawns, state.all_other);

  while (pawn_moves) {
    auto [to_square, to_bb] = pawn_moves.index_bb_and_clear();
    auto to_piece = state.other.find_type(to_bb);
    assert(to_piece.type() != Type::None);
    auto from_square = from_fn(to_square);

    if (not is_promo_fn(to_square))
      moves.emplace_back(Piece::pawn(), from_square, to_piece, to_square);
    else {
      moves.push_back(Move::PawnPromo(
            from_square, to_piece, to_square, Piece::queen()));
      moves.push_back(Move::PawnPromo(
            from_square, to_piece, to_square, Piece::rook()));
      moves.push_back(Move::PawnPromo(
            from_square, to_piece, to_square, Piece::bishop()));
      moves.push_back(Move::PawnPromo(
            from_square, to_piece, to_square, Piece::knight()));
    }
  }
}

// We create a BitBoard with one bit set where the pawn moves after capturing
// en passant, and use that to check if there are any captures.
void
move_en_passant(
    const BoardState& state,
    std::vector<Move>& moves)
{
  if (not state.en_passant)
    return;

  // Fake square
  auto pawns = state.mine.pawn();
  unsigned to_sq = state.en_passant_file;
  unsigned passant_sq = state.en_passant_file;

  if (state.is_white_next()) {
    to_sq += 40; // 6th row
    passant_sq += 32; // 5th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = MoveWhitePawnsAttackLeft(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_white(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = MoveWhitePawnsAttackRight(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_white(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  } else {
    to_sq += 16; // 3rd row
    passant_sq += 24; // 4th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = MoveBlackPawnsAttackLeft(pawns, to_bb);
    if (attack) {
      auto from_sq = from_left_black(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = MoveBlackPawnsAttackRight(pawns, to_bb);
    if (attack) {
      auto from_sq = from_right_black(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  }
}

} // namespace

std::vector<Move>
MoveGen::for_king(const BoardState& state) const
{
  std::vector<Move> moves;
  for_king(state, moves);
  return moves;
}

// Generates all possible moves for queens on the board.
std::vector<Move>
MoveGen::for_queen(const BoardState& state) const
{
  std::vector<Move> moves;
  for_queen(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::for_rook(const BoardState& state) const
{
  std::vector<Move> moves;
  for_rook(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::for_bishop(const BoardState& state) const
{
  std::vector<Move> moves;
  for_bishop(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::for_knight(const BoardState& state) const
{
  return get_simple_moves(Piece::knight(), state, MoveKnight);
}

std::vector<Move>
MoveGen::for_pawn(const BoardState& state) const
{
  std::vector<Move> moves;
  for_pawn(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::for_all(const BoardState& state) const
{
  // TODO: determine if there are any performance gains from computing the moves
  // in parallel.
  std::vector<Move> moves;
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
    const BoardState& state,
    std::vector<Move>& moves) const
{
  get_simple_moves(Piece::king(), state, MoveKing, moves);

  auto all_pieces = state.all_mine | state.all_other;

  switch (state.next) {
  case Color::White:
    if (state.wk_castle and
        can_castle<Color::White, BoardSide::King>(all_pieces))
      moves.push_back(Move::WhiteKingSideCastle());
    if (state.wq_castle and
        can_castle<Color::White, BoardSide::Queen>(all_pieces))
      moves.push_back(Move::WhiteQueenSideCastle());
    break;
  case Color::Black:
    if (state.bk_castle and
        can_castle<Color::Black, BoardSide::King>(all_pieces))
      moves.push_back(Move::BlackKingSideCastle());
    if (state.bq_castle and
        can_castle<Color::Black, BoardSide::Queen>(all_pieces))
      moves.push_back(Move::BlackQueenSideCastle());
    break;
  }
}

void
MoveGen::for_queen(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics_.get_attacks(from_square, all_pieces)
         | bmagics_.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::queen(), state, moves_fn, moves);
}

void
MoveGen::for_rook(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics_.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::rook(), state, moves_fn, moves);
}

void
MoveGen::for_bishop(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return bmagics_.get_attacks(from_square, all_pieces);
  };
  get_simple_moves(Piece::bishop(), state, moves_fn, moves);
}

void
MoveGen::for_knight(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  get_simple_moves(Piece::knight(), state, MoveKnight, moves);
}

void
MoveGen::for_pawn(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  BitBoard pawns = state.mine.pawn();

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

  switch (state.next) {
  case Color::White:
    single_fn = MoveWhitePawnsSingle;
    double_fn = MoveWhitePawnsDouble;
    attack_left_fn = MoveWhitePawnsAttackLeft;
    attack_right_fn = MoveWhitePawnsAttackRight;
    from_single_fn = from_single_white;
    from_double_fn = from_double_white;
    from_left_fn = from_left_white;
    from_right_fn = from_right_white;
    is_promo_fn = is_white_promo;
    break;
  default:
    single_fn = MoveBlackPawnsSingle;
    double_fn = MoveBlackPawnsDouble;
    attack_left_fn = MoveBlackPawnsAttackLeft;
    attack_right_fn = MoveBlackPawnsAttackRight;
    from_single_fn = from_single_black;
    from_double_fn = from_double_black;
    from_left_fn = from_left_black;
    from_right_fn = from_right_black;
    is_promo_fn = is_black_promo;
    break;
  }

  auto no_pieces = ~(state.all_mine | state.all_other);

  // TODO: handle en passant moves
  move_forward(pawns, no_pieces, single_fn, from_single_fn, is_promo_fn, moves);
  move_forward(pawns, no_pieces, double_fn, from_double_fn, is_promo_fn, moves);
  attack(pawns, state, attack_left_fn, from_left_fn, is_promo_fn, moves);
  attack(pawns, state, attack_right_fn, from_right_fn, is_promo_fn, moves);
  move_en_passant(state, moves);
}

} // namespace blunder
