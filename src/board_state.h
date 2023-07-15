#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <ostream>
#include <string>

#include "bitboard.h"
#include "board_side.h"
#include "color.h"
#include "pieces.h"
#include "piece_set.h"

namespace blunder {

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

// Forward declaration because we will make this a friend of BoardState to
// make it easier to build BoardState while preserving invariants.
class BoardStateBuilder;

// BoardState represents the current state of the board. Some of the fields are
// written from the perspective of the player moving next to simplify move
// generation.
class BoardState {
public:
  // Returns a reference to piece set for player moving next.
  const PieceSet& mine() const noexcept
  { return bb_mine; }

  // Returns a reference to piece set for player not moving next.
  const PieceSet& other() const noexcept
  { return bb_other; }

  // Returns true if white is next to move.
  bool
  is_white_next() const noexcept
  { return next == Color::White; }

  // Returns a bitboard with all the pieces for the player moving next.
  BitBoard
  all_mine() const noexcept
  { return mine().full_set(); }

  // Returns a bitboard with all the pieces for the player not moving next.
  BitBoard
  all_other() const noexcept
  { return other().full_set(); }

  // Returns a bitboard with all the pieces both players.
  BitBoard
  all_bits() const noexcept
  { return all_mine() | all_other(); }

  BitBoard
  none() const noexcept
  { return all_bits().bit_not(); }

  const PieceSet&
  white() const noexcept
  { return is_white_next() ? bb_mine : bb_other; }

  const PieceSet&
  black() const noexcept
  { return is_white_next() ? bb_other : bb_mine; }

  unsigned
  hm_count() const noexcept
  { return half_move; }

  unsigned
  fm_count() const noexcept
  { return full_move; }

  std::string
  str() const;

  bool
  eq(const BoardState& other) const noexcept;

  // Initializes a BoardState for a new game.
  static BoardState
  new_board() noexcept;

  // Returns true if there is an opportunity for a pawn capture by en passant
  // in the current move.
  bool
  has_enpassant() const noexcept
  { return en_passant; }

  // Returns file number where a pawn can be captured by en passant.
  unsigned
  enpassant_file() const noexcept
  { return en_passant_file; }

  //---------------------------
  // Check for castling rights.
  //---------------------------

  bool
  has_wk_castling() const noexcept
  { return wk_castle; }

  unsigned
  has_wq_castling() const noexcept
  { return wq_castle; }

  unsigned
  has_bk_castling() const noexcept
  { return bk_castle; }

  unsigned
  has_bq_castling() const noexcept
  { return bq_castle; }

  //-------------------------------
  // Check if castling is possible.
  //-------------------------------

  unsigned
  wk_can_castle() const noexcept
  { return wk_castle
       and can_castle<Color::White, BoardSide::King>(all_bits()); }

  unsigned
  wq_can_castle() const noexcept
  { return wq_castle
       and can_castle<Color::White, BoardSide::Queen>(all_bits()); }

  unsigned
  bk_can_castle() const noexcept
  { return bk_castle
       and can_castle<Color::Black, BoardSide::King>(all_bits()); }

  unsigned
  bq_can_castle() const noexcept
  { return bq_castle
       and can_castle<Color::Black, BoardSide::Queen>(all_bits()); }

private:
  friend BoardStateBuilder;

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
  // |bb_mine| are the pieces for the player to move next, and |bb_other| are
  // the pieces for the player who just moved.
  PieceSet bb_mine;
  PieceSet bb_other;

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
};

inline bool
operator==(const BoardState& left, const BoardState& right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, const BoardState& state)
{ return os << state.str(); }

enum class BoardStateErr {
  // White pieces are not valid.
  White,
  // Black pieces are not valid.
  Black,
  // Too many half moves.
  HalfMove,
  // En passant file is not valid.
  EnPassantFile
};

class BoardStateBuilder {
public:
  std::expected<BoardState, BoardStateErr>
  build() const noexcept
  {
    if (!state.white().is_valid())
      return std::unexpected(BoardStateErr::White);

    if (!state.black().is_valid())
      return std::unexpected(BoardStateErr::White);

    if (file_err)
      return std::unexpected(BoardStateErr::EnPassantFile);

    if (half_move_err)
      return std::unexpected(BoardStateErr::HalfMove);

    return state;
  }

  // All pieces are set simulateously to make sure that we set mine and other
  // correctly with respect to the color to move next.
  BoardStateBuilder&
  set_pieces(Color color, const PieceSet& white, const PieceSet& black) noexcept
  {
    if (color == Color::White) {
      state.bb_mine = white;
      state.bb_other = black;
    } else {
      state.bb_mine = black;
      state.bb_other = white;
    }
    state.next = color;
    return *this;
  }

  BoardStateBuilder&
  set_half_move(unsigned hm) noexcept
  {
    if (hm > 100) {
      half_move_err = true;
      return *this;
    }

    half_move_err = false;
    state.half_move = hm;
    return *this;
  }

  BoardStateBuilder&
  set_full_move(unsigned fm) noexcept
  {
    state.full_move = fm;
    return *this;
  }

  BoardStateBuilder&
  set_enpassant_file(unsigned file) noexcept
  {
    if (file >= 8) {
      file_err = true;
      return *this;
    }

    file_err = false;
    state.en_passant = 1;
    state.en_passant_file = file;

    return *this;
  }

  BoardStateBuilder&
  set_wk_castling(bool has_right) noexcept
  {
    state.wk_castle = has_right;
    return *this;
  }

  BoardStateBuilder&
  set_wq_castling(bool has_right) noexcept
  {
    state.wq_castle = has_right;
    return *this;
  }

  BoardStateBuilder&
  set_bk_castling(bool has_right) noexcept
  {
    state.bk_castle = has_right;
    return *this;
  }

  BoardStateBuilder&
  set_bq_castling(bool has_right) noexcept
  {
    state.bq_castle = has_right;
    return *this;
  }

private:
  BoardState state;
  bool file_err = false;
  bool half_move_err = false;
};

} // namespace blunder
