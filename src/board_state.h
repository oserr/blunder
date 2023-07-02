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
  White() const noexcept
  { return next == Color::White ? mine : other; }

  BitBoard
  WhiteKing() const noexcept
  { return White().king(); }

  BitBoard
  WhiteQueen() const noexcept
  { return White().queen(); }

  BitBoard
  WhiteRook() const noexcept
  { return White().rook(); }

  BitBoard
  WhiteBishop() const noexcept
  { return White().bishop(); }

  BitBoard
  WhiteKnight() const noexcept
  { return White().knight(); }

  BitBoard
  WhitePawn() const noexcept
  { return White().pawn(); }

  unsigned
  NumWhiteQueen() const noexcept
  { return WhiteQueen().count(); }

  unsigned
  NumWhiteRook() const noexcept
  { return WhiteRook().count(); }

  unsigned
  NumWhiteBishop() const noexcept
  { return WhiteBishop().count(); }

  unsigned
  NumWhiteKnight() const noexcept
  { return WhiteKnight().count(); }

  unsigned
  NumWhitePawn() const noexcept
  { return WhitePawn().count(); }

  unsigned
  NumWhite() const noexcept
  { return White().full_set().count(); }

  const PieceSet&
  Black() const noexcept
  { return next == Color::White ? other : mine; }

  BitBoard
  BlackKing() const noexcept
  { return Black().king(); }

  BitBoard
  BlackQueen() const noexcept
  { return Black().queen(); }

  BitBoard
  BlackRook() const noexcept
  { return Black().rook(); }

  BitBoard
  BlackBishop() const noexcept
  { return Black().bishop(); }

  BitBoard
  BlackKnight() const noexcept
  { return Black().knight(); }

  BitBoard
  BlackPawn() const noexcept
  { return Black().pawn(); }

  unsigned
  NumBlackQueen() const noexcept
  { return BlackQueen().count(); }

  unsigned
  NumBlackRook() const noexcept
  { return BlackRook().count(); }

  unsigned
  NumBlackBishop() const noexcept
  { return BlackBishop().count(); }

  unsigned
  NumBlackKnight() const noexcept
  { return BlackKnight().count(); }

  unsigned
  NumBlackPawn() const noexcept
  { return BlackPawn().count(); }

  unsigned
  NumBlack() const noexcept
  { return Black().full_set().count(); }
};

// Initializes a BoardState for a new game.
BoardState
NewBoardState() noexcept;

bool
operator==(const BoardState& left, const BoardState& right) noexcept;

std::string
DebugStr(const BoardState& state);

inline std::ostream&
operator<<(std::ostream& os, const BoardState& state)
{
  os << DebugStr(state);
  return os;
}

} // namespace blunder
