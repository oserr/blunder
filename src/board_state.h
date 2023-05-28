#pragma once

#include <array>
#include <cstdint>

#include "bitboard.h"
#include "color.h"

namespace blunder {

// BoardState represents the current state of the board. Some of the fields are
// written from the perspective of the player moving next to simplify move
// generation.
struct BoardState {
  // Arrays of bitboards for all pieces, in the following order:
  // - King
  // - Queens
  // - Rooks
  // - Bishops
  // - Knights
  // - Pawns
  //
  // If a piece is no longer on the board, the bitboard will be set to 0.
  //
  // |mine| are the pieces for the player to move next, and |other| are the
  // pieces for the player who just moved.
  std:array<BitBoard, 6> mine;
  std:array<BitBoard, 6> other;

  // All of the pieces for the player next to move.
  BitBoard all_mine = 0;

  // All of the pieces for the player who just moved.
  BitBoard all_other = 0;

  // The next color to move.
  Color next;

  // If set, indicates that en passant is possible.
  std::uint8_t en_passant: 1 = 0;

  // Bit field to indicate the file where en passant is possible.
  std::uint8_t en_passant_file: 3 = 0;

  // Indicates if white can castle on kingside.
  std::uint8_t wk_castle: 1 = 1;

  // Indicates if white can castle on queenside.
  std::uint8_t wq_castle: 1 = 1;

  // Indicates if black can castle on kingside.
  std::uint8_t bk_castle: 1 = 1;

  // Indicates if black can castle on queenside.
  std::uint8_t bq_castle: 1 = 1;
};

} // namespace blunder
