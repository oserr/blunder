#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "pieces.h"
#include "square.h"

namespace blunder {

// Move contains multiple bitfields to represent a chess move, taking less than
// 4 total bytes.
struct Move {
  // Represents the piece being moved.
  // - 0: King
  // - 1: Queen
  // - 2: Rook
  // - 3: Bishop
  // - 4: Knight
  // - 5: Pawn
  // - 6: None
  //
  // If the move does not represent an attack, then |to_piece| should be set to
  // None.
  std::uint8_t from_piece: 3 = 0;
  std::uint8_t to_piece: 3 = to_int(Type::None);

  // The source and destination squares.
  std::uint8_t from_square: 6 = 0;
  std::uint8_t to_square: 6 = 0;

  // The square of the captured piece on an en-passant move.
  std::uint8_t passant_square: 6 = 0;

  // For king moves, indicates if the king is castling. kside=1 indicates
  // kingside, and kside=0 indicates queenside.
  std::uint8_t castle: 1 = 0;
  std::uint8_t kside: 1 = 0;

  // For pawn moves, indicates en-passant and whether the move is a promotion.
  // If it is, promo_piece will be set to promoted piece.
  std::uint8_t en_passant: 1 = 0;
  std::uint8_t is_promo: 1 = 0;
  std::uint8_t promo_piece: 3 = to_int(Type::None);

  // Initializes all fields.
  Move() = default;

  // ------------------------------------------------------------------------
  // Most common scenario for a move, i.e. we move a piece from one square to
  // another without attacking, and hence this initializes the move with the
  // source square, piece type for moving piece, and destination square.
  // ------------------------------------------------------------------------

  Move(
      std::uint8_t from_piece,
      std::uint8_t from_square,
      std::uint8_t to_square) noexcept
    : from_piece(from_piece),
      from_square(from_square),
      to_square(to_square) {}

  Move(
      Piece from_piece,
      std::uint8_t from_square,
      std::uint8_t to_square) noexcept
    : Move(from_piece.uint(), from_square, to_square) {}

  Move(
      Piece from_piece,
      Sq from_square,
      Sq to_square) noexcept
    : Move(from_piece.uint(), to_int(from_square), to_int(to_square)) {}

  // ------------------------------------------------------------------------
  // Next most common scenario, we move a piece with capture, with overloads.
  // ------------------------------------------------------------------------

  Move(
      std::uint8_t from_piece,
      std::uint8_t from_square,
      std::uint8_t to_piece,
      std::uint8_t to_square) noexcept
    : from_piece(from_piece),
      to_piece(to_piece),
      from_square(from_square),
      to_square(to_square) {}

  Move(
      Piece from_piece,
      std::uint8_t from_square,
      std::uint8_t to_piece,
      std::uint8_t to_square) noexcept
    : Move(from_piece.uint(), from_square, to_piece, to_square) {}

  Move(
      Piece from_piece,
      std::uint8_t from_square,
      Piece to_piece,
      std::uint8_t to_square) noexcept
    : Move(from_piece.uint(), from_square, to_piece.uint(), to_square) {}

  // Copy control.
  constexpr Move(const Move& m) noexcept = default;
  Move& operator=(const Move& m) noexcept = default;

  // --------------------------------------------------------------------------
  // Convenience functions to initialize special types of moves, like castling,
  // pawn promos, etc.
  // --------------------------------------------------------------------------

  // Returns a move for white king side castle.
  static constexpr Move
  wk_castle() noexcept
  {
    Move m(Piece::king(), 4, 6);
    m.castle = 1;
    m.kside = 1;
    return m;
  }

  // Returns a move for white queen side castle.
  static constexpr Move
  wq_castle() noexcept
  {
    Move m(Piece::king(), 4, 2);
    m.castle = 1;
    return m;
  }

  // Returns a move for black king side castle.
  static constexpr Move
  bk_castle() noexcept
  {
    Move m(Piece::king(), 60, 62);
    m.castle = 1;
    m.kside = 1;
    return m;
  }

  // Returns a move for black queen side castle.
  static constexpr Move
  bq_castle() noexcept
  {
    Move m(Piece::king(), 60, 58);
    m.castle = 1;
    return m;
  }

  // Returns a move for pawn promotion without capture.
  static Move
  promo(
      std::uint8_t from_sq,
      std::uint8_t to_sq,
      Piece promo) noexcept
  {
    assert(from_sq < 64);
    assert(to_sq < 64);

    Move m(Piece::pawn(), from_sq, to_sq);
    m.is_promo = 1;
    m.promo_piece = promo.uint();
    return m;
  }

  // Returns a move for pawn promotion with capture.
  static Move
  promo(
      std::uint8_t from_sq,
      std::uint8_t to_piece,
      std::uint8_t to_sq,
      Piece promo) noexcept
  {
    assert(from_sq < 64);
    assert(to_sq < 64);

    Move m(Piece::pawn(), from_sq, to_piece, to_sq);
    m.is_promo = 1;
    m.promo_piece = promo.uint();
    return m;
  }

  // Returns a move for pawn promotion with capture.
  static Move
  promo(
      std::uint8_t from_sq,
      Piece to_piece,
      std::uint8_t to_sq,
      Piece promo) noexcept
  { return Move::promo(from_sq, to_piece.uint(), to_sq, promo); }

  // Returns a move for en passant capture.
  static Move
  by_en_passant(
      std::uint8_t from_sq,
      std::uint8_t to_sq,
      std::uint8_t passant_sq) noexcept
  {
    assert(from_sq < 64);
    assert(to_sq < 64);

    Move m(Piece::pawn(), from_sq, to_sq);
    m.passant_square = passant_sq;
    m.en_passant = true;
    return m;
  }

  // Cretes a printable debug string for Move in the form
  //
  //   {P:from_square -> to_square, ...}
  //
  // where P is a color agnostic piece type, and ... may contain the rook move
  // in a castling move, a captured piece !P, and a promoted pawn piece ^P.
  std::string
  str() const;

  // Returns true if both moves are equal.
  bool
  eq(Move mv) const noexcept;
};

inline bool
operator==(Move left, Move right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, Move mv)
{ return os << mv.str(); }

using MoveVec = std::vector<Move>;

} // namespace blunder
