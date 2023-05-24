#pragma once

#include <cstdint>

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
  std::uint8_t from_piece: 3;
  std::uint8_t to_piece: 3;

  // The source and destination squares.
  std::uint8_t from_square: 6;
  std::uint8_t to_square: 6;

  // For pawn moves, indicates en-passant.
  std::uint8_t en_passant: 1;

  // For king moves, indicates if the king is castling. If the king is castling,
  // file=0 indicates the rook moving is from file A, and file=1 indicates from
  // file H.
  std::uint8_t castle: 1;
  std::uint8_t file: 1;
};

} // namespace blunder
