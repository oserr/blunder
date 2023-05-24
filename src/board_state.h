#pragma once

#include <array>
#include <cstdint>

#include "bitboard.h"
#include "color.h"

namespace blunder {

// BoardState represents the current state of the board.
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
  std:array<BitBoard, 6> white;
  std:array<BitBoard, 6> black;

  // All white pieces.
  BitBoard all_white = 0;

  // All black pieces.
  BitBoard all_black = 0;

  Color next_to_move;

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
