#pragma once

#include <cstdint>

#include "pieces.h"

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
  std::uint8_t to_piece: 3 = 0;

  // The source and destination squares.
  std::uint8_t from_square: 6 = 0;
  std::uint8_t to_square: 6 = 0;

  // For pawn moves, indicates en-passant.
  std::uint8_t en_passant: 1 = 0;

  // For king moves, indicates if the king is castling. If the king is castling,
  // file=0 indicates the rook moving is from file A, and file=1 indicates from
  // file H.
  std::uint8_t castle: 1 = 0;
  std::uint8_t file: 1 = 0;

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

  // Next most common scenario, we move a piece with capture.
  constexpr Move(
      std::uint8_t from_piece,
      std::uint8_t from_square,
      std::uint8_t to_square
      std::uint8_t to_piece) noexcept
    : from_piece(from_piece),
      from_square(from_square),
      to_square(to_square),
      to_piece(to_piece) {}

  // Default assignment operator.
  Move operator=(Move m) noexcept = default;

  static constexpr Move
  WhiteKingSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 4, 6);
    m.castle = 1;
    m.file = 1;
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
    m.file = 1;
    return m;
  }

  static constexpr Move
  BlackQueenSideCastle() noexcept
  {
    Move m(Uint8(Piece::King), 60, 58);
    m.castle = 1;
    return m;
  }
};

} // namespace blunder
