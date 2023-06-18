#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

#include "bitboard.h"
#include "color.h"
#include "pieces.h"

namespace blunder {

using PieceSet = std::array<BitBoard, 6>;

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
  BitBoard all_mine = 0;

  // All of the pieces for the player who just moved.
  BitBoard all_other = 0;

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
  WhiteKiings() const noexcept
  { return White()[Uint8(Piece::King)]; }

  BitBoard
  WhiteQueens() const noexcept
  { return White()[Uint8(Piece::Queen)]; }

  BitBoard
  WhiteRooks() const noexcept
  { return White()[Uint8(Piece::Rook)]; }

  BitBoard
  WhiteBishops() const noexcept
  { return White()[Uint8(Piece::Bishop)]; }

  BitBoard
  WhiteKnights() const noexcept
  { return White()[Uint8(Piece::Knight)]; }

  BitBoard
  WhitePawns() const noexcept
  { return White()[Uint8(Piece::Pawn)]; }

  unsigned
  NumWhiteQueens() const noexcept
  { return std::popcount(WhiteQueens()); }

  unsigned
  NumWhiteRooks() const noexcept
  { return std::popcount(WhiteRooks()); }

  unsigned
  NumWhiteBishops() const noexcept
  { return std::popcount(WhiteBishops()); }

  unsigned
  NumWhiteKnights() const noexcept
  { return std::popcount(WhiteKnights()); }

  unsigned
  NumWhitePawns() const noexcept
  { return std::popcount(WhitePawns()); }

  unsigned
  NumWhite() const noexcept
  { return std::popcount(AllMask(White())); }

  const PieceSet&
  Black() const noexcept
  { return next == Color::White ? other : mine; }

  BitBoard
  BlackKing() const noexcept
  { return Black()[Uint8(Piece::King)]; }

  BitBoard
  BlackQueens() const noexcept
  { return Black()[Uint8(Piece::Queen)]; }

  BitBoard
  BlackRooks() const noexcept
  { return Black()[Uint8(Piece::Rook)]; }

  BitBoard
  BlackBishops() const noexcept
  { return Black()[Uint8(Piece::Bishop)]; }

  BitBoard
  BlackKnights() const noexcept
  { return Black()[Uint8(Piece::Knight)]; }

  BitBoard
  BlackPawns() const noexcept
  { return Black()[Uint8(Piece::Pawn)]; }

  unsigned
  NumBlackQueens() const noexcept
  { return std::popcount(BlackQueens()); }

  unsigned
  NumBlackRooks() const noexcept
  { return std::popcount(BlackRooks()); }

  unsigned
  NumBlackBishops() const noexcept
  { return std::popcount(BlackBishops()); }

  unsigned
  NumBlackKnights() const noexcept
  { return std::popcount(BlackKnights()); }

  unsigned
  NumBlackPawns() const noexcept
  { return std::popcount(BlackPawns()); }

  unsigned
  NumBlack() const noexcept
  { return std::popcount(AllMask(Black())); }

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
