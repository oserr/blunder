#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

#include "bitboard.h"
#include "color.h"
#include "pieces.h"
#include "piece_set.h"

namespace blunder {

// Computes the union of all the bitboards in |pieces|.
BitBoard
AllMask(const PieceSet& pieces) noexcept;

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
  PieceSet mine;
  PieceSet other;

  // All of the pieces for the player next to move.
  BitBoard all_mine;

  // All of the pieces for the player who just moved.
  BitBoard all_other;

  std::uint16_t half_move = 0;
  std::uint16_t full_move = 0;

  // The next color to move.
  Color next;

  // If set, indicates that en passant is possible.
  std::uint8_t en_passant: 1 = 0;

  // Bit field to indicate the file where en passant is possible.
  std::uint8_t en_passant_file: 3 = 0;

  // Indicates if white can castle on kingside.
  std::uint8_t wk_castle: 1 = 0;

  // Indicates if white can castle on queenside.
  std::uint8_t wq_castle: 1 = 0;

  // Indicates if black can castle on kingside.
  std::uint8_t bk_castle: 1 = 0;

  // Indicates if black can castle on queenside.
  std::uint8_t bq_castle: 1 = 0;

  bool in_check = false;

  bool
  is_white_next() const noexcept
  { return next == Color::White; }

  // Sets |all_mine|, i.e. a BitBoard with all pieces from |mine|.
  void
  SetAllMine() noexcept
  { all_mine = AllMask(mine); }

  // Sets |all_other|, i.e. a BitBoard with all pieces from |other|.
  void
  SetAllOther() noexcept
  { all_other = AllMask(other); }

  const PieceSet&
  white() const noexcept
  { return next == Color::White ? mine : other; }

  const PieceSet&
  black() const noexcept
  { return next == Color::White ? other : mine; }

  std::string
  str() const;

  bool
  eq(const BoardState& other) const noexcept;
};

// Initializes a BoardState for a new game.
BoardState
NewBoardState() noexcept;

inline bool
operator==(const BoardState& left, const BoardState& right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, const BoardState& state)
{ return os << state.str(); }

} // namespace blunder
