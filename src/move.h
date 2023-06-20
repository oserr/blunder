#pragma once

#include <cstdint>
#include <string>

#include "pieces.h"
#include "square.h"

namespace blunder {

// Move contains multiple bitfields to represent a chess move.
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
  std::uint8_t to_piece: 3 = Uint8(Piece::None);

  // The source and destination squares.
  std::uint8_t from_square: 6 = 0;
  std::uint8_t to_square: 6 = 0;

  // For king moves, indicates if the king is castling. kside=1 indicates
  // kingside, and kside=0 indicates queenside.
  std::uint8_t castle: 1 = 0;
  std::uint8_t kside: 1 = 0;

  // For pawn moves, indicates en-passant and whether the move is a promotion.
  // If it is, promo_piece will be set to promoted piece.
  std::uint8_t en_passant: 1 = 0;
  std::uint8_t is_promo: 1 = 0;
  std::uint8_t promo_piece: 3 = Uint8(Piece::None);

  // Initializes all fields.
  constexpr Move() = default;

  // Most common scenario for a move, i.e. we move a piece from one square to
  // another without attacking.
  constexpr Move(
      std::uint8_t from_piece,
      std::uint8_t from_square,
      std::uint8_t to_square) noexcept
    : from_piece(from_piece),
      from_square(from_square),
      to_square(to_square) {}

  // Most common scenario for a move, i.e. we move a piece from one square to
  // another without attacking.
  constexpr Move(
      Piece from_piece,
      Sq from_square,
      Sq to_square) noexcept
    : Move(Uint8(from_piece), ToUint(from_square), ToUint(to_square)) {}

  // Next most common scenario, we move a piece with capture.
  constexpr Move(
      std::uint8_t from_piece,
      std::uint8_t from_square,
      std::uint8_t to_piece,
      std::uint8_t to_square) noexcept
    : from_piece(from_piece),
      to_piece(to_piece),
      from_square(from_square),
      to_square(to_square) {}

  // Copy ctor.
  constexpr Move(const Move& m) noexcept = default;

  // Default assignment operator.
  Move& operator=(const Move& m) noexcept = default;

  static constexpr Move
  WhiteKingSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 4, 6);
    m.castle = 1;
    m.kside = 1;
    return m;
  }

  static constexpr Move
  WhiteQueenSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 4, 2);
    m.castle = 1;
    return m;
  }

  static constexpr Move
  BlackKingSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 60, 62);
    m.castle = 1;
    m.kside = 1;
    return m;
  }

  static constexpr Move
  BlackQueenSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 60, 58);
    m.castle = 1;
    return m;
  }

  static Move
  WhitePawnPromo(std::uint8_t to_square, Piece promo) noexcept
  {
    assert(to_square < 64);

    Move m(Uint8(Piece::Pawn), to_square - 8, to_square);
    m.is_promo = 1;
    m.promo_piece = Uint8(promo);
    return m;
  }

  static Move
  WhitePawnPromo(
      std::uint8_t from_square,
      std::uint8_t to_piece,
      std::uint8_t to_square,
      Piece promo) noexcept
  {
    assert(to_square < 64);

    Move m(Uint8(Piece::Pawn), from_square, to_piece, to_square);
    m.is_promo = 1;
    m.promo_piece = Uint8(promo);
    return m;
  }

  static Move
  PawnPromo(
      std::uint8_t from_square,
      std::uint8_t to_square,
      Piece promo) noexcept
  {
    assert(to_square < 64);

    Move m(Uint8(Piece::Pawn), from_square, to_square);
    m.is_promo = 1;
    m.promo_piece = Uint8(promo);
    return m;
  }

  static Move
  PawnPromo(
      std::uint8_t from_square,
      std::uint8_t to_piece,
      std::uint8_t to_square,
      Piece promo) noexcept
  {
    assert(to_square < 64);

    Move m(Uint8(Piece::Pawn), from_square, to_piece, to_square);
    m.is_promo = 1;
    m.promo_piece = Uint8(promo);
    return m;
  }

};

bool
operator==(const Move& left, const Move& right) noexcept;

// Cretes a printable debug string for Move in the form
//
//   {P:from_square -> to_square, ...}
//
// where P is a color agnostic piece type, and ... may contain the rook move in
// a castling move, a captured piece !P, and a promoted pawn piece ^P.
std::string
DebugStr(const Move& mv);

inline std::ostream&
operator<<(std::ostream& os, const Move& mv)
{
  os << DebugStr(mv);
  return os;
}

} // namespace blunder
