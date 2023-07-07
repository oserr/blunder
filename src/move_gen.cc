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
GetSimpleAttacks(
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
GetSimpleMoves(
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

using PawnMovesFn = BitBoard(*)(BitBoard, BitBoard);
using FromFn = std::uint8_t(*)(std::uint8_t);
using IsPromoFn = bool(*)(std::uint8_t);

bool
IsWhitePromo(std::uint8_t to_square) noexcept
{ return to_square >= 56; }

bool
IsBlackPromo(std::uint8_t to_square) noexcept
{ return to_square <= 7; }

std::uint8_t
FromSingleWhite(std::uint8_t to_square) noexcept
{ return to_square - 8; }

std::uint8_t
FromDoubleWhite(std::uint8_t to_square) noexcept
{ return to_square - 16; }

std::uint8_t
FromLeftWhite(std::uint8_t to_square) noexcept
{ return to_square - 7; }

std::uint8_t
FromRightWhite(std::uint8_t to_square) noexcept
{ return to_square - 9; }

std::uint8_t
FromSingleBlack(std::uint8_t to_square) noexcept
{ return to_square + 8; }

std::uint8_t
FromDoubleBlack(std::uint8_t to_square) noexcept
{ return to_square + 16; }

std::uint8_t
FromLeftBlack(std::uint8_t to_square) noexcept
{ return to_square + 7; }

std::uint8_t
FromRightBlack(std::uint8_t to_square) noexcept
{ return to_square + 9; }

void
MovePawnsForward(
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
MovePawnsAttack(
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
      auto from_sq = FromLeftWhite(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = MoveWhitePawnsAttackRight(pawns, to_bb);
    if (attack) {
      auto from_sq = FromRightWhite(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  } else {
    to_sq += 16; // 3rd row
    passant_sq += 24; // 4th row

    auto to_bb = BitBoard::from_index(to_sq);

    auto attack = MoveBlackPawnsAttackLeft(pawns, to_bb);
    if (attack) {
      auto from_sq = FromLeftBlack(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }

    attack = MoveBlackPawnsAttackRight(pawns, to_bb);
    if (attack) {
      auto from_sq = FromRightBlack(to_sq);
      moves.push_back(Move::by_en_passant(from_sq, to_sq, passant_sq));
    }
  }
}

} // namespace

std::vector<Move>
MoveGen::KingMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  KingMoves(state, moves);
  return moves;
}

// Generates all possible moves for queens on the board.
std::vector<Move>
MoveGen::QueenMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  QueenMoves(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::RookMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  RookMoves(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::BishopMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  BishopMoves(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::KnightMoves(const BoardState& state) const
{
  return GetSimpleMoves(Piece::knight(), state, MoveKnight);
}

std::vector<Move>
MoveGen::PawnMoves(const BoardState& state) const
{
  std::vector<Move> moves;
  PawnMoves(state, moves);
  return moves;
}

std::vector<Move>
MoveGen::AllMoves(const BoardState& state) const
{
  // TODO: determine if there are any performance gains from computing the moves
  // in parallel.
  std::vector<Move> moves;
  KingMoves(state, moves);
  QueenMoves(state, moves);
  RookMoves(state, moves);
  BishopMoves(state, moves);
  KnightMoves(state, moves);
  PawnMoves(state, moves);
  return moves;
}

void
MoveGen::KingMoves(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  GetSimpleMoves(Piece::king(), state, MoveKing, moves);

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
MoveGen::QueenMoves(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics_.get_attacks(from_square, all_pieces)
         | bmagics_.get_attacks(from_square, all_pieces);
  };
  GetSimpleMoves(Piece::queen(), state, moves_fn, moves);
}

void
MoveGen::RookMoves(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return rmagics_.get_attacks(from_square, all_pieces);
  };
  GetSimpleMoves(Piece::rook(), state, moves_fn, moves);
}

void
MoveGen::BishopMoves(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  auto all_pieces = state.all_mine | state.all_other;
  auto moves_fn = [&](BitBoard bb) {
    auto from_square = bb.first_bit();
    return bmagics_.get_attacks(from_square, all_pieces);
  };
  GetSimpleMoves(Piece::bishop(), state, moves_fn, moves);
}

void
MoveGen::KnightMoves(
    const BoardState& state,
    std::vector<Move>& moves) const
{
  GetSimpleMoves(Piece::knight(), state, MoveKnight, moves);
}

void
MoveGen::PawnMoves(
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
    from_single_fn = FromSingleWhite;
    from_double_fn = FromDoubleWhite;
    from_left_fn = FromLeftWhite;
    from_right_fn = FromRightWhite;
    is_promo_fn = IsWhitePromo;
    break;
  default:
    single_fn = MoveBlackPawnsSingle;
    double_fn = MoveBlackPawnsDouble;
    attack_left_fn = MoveBlackPawnsAttackLeft;
    attack_right_fn = MoveBlackPawnsAttackRight;
    from_single_fn = FromSingleBlack;
    from_double_fn = FromDoubleBlack;
    from_left_fn = FromLeftBlack;
    from_right_fn = FromRightBlack;
    is_promo_fn = IsBlackPromo;
    break;
  }

  auto no_pieces = ~(state.all_mine | state.all_other);

  // TODO: handle en passant moves
  MovePawnsForward(
      pawns, no_pieces, single_fn, from_single_fn, is_promo_fn, moves);
  MovePawnsForward(
      pawns, no_pieces, double_fn, from_double_fn, is_promo_fn, moves);
  MovePawnsAttack(
      pawns, state, attack_left_fn, from_left_fn, is_promo_fn, moves);
  MovePawnsAttack(
      pawns, state, attack_right_fn, from_right_fn, is_promo_fn, moves);
  move_en_passant(state, moves);
}

} // namespace blunder
