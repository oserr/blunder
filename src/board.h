#pragma once

#include <cassert>
#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "bitboard.h"
#include "board_side.h"
#include "color.h"
#include "magics.h"
#include "move.h"
#include "moves.h"
#include "pieces.h"
#include "piece_set.h"

namespace blunder {

// Forward declaration because we will make this a friend of Board to
// make it easier to build Board while preserving invariants.
class BoardBuilder;

// Board represents the current state of the board. Some of the fields are
// written from the perspective of the player moving next to simplify move
// generation.
class Board {
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
  eq(const Board& other) const noexcept;

  // Initializes a Board for a new game.
  static Board
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

  // Returns a BitBoard with all of the squares attacked by other set.
  BitBoard
  get_attacked() const noexcept
  { return bb_attacked; }

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

private:
  // Used internally to check if castling is possible for a color and a side.
  template<Color color, BoardSide side>
  bool
  can_castle() const noexcept
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

    // Here we treat attacked squares as if they were occupied.
    auto all_pieces = all_bits() | get_attacked();

    if constexpr (color == Color::Black)
      all_pieces >>= 56;

    all_pieces &= 0xffull;
    all_pieces &= mask;

    return bits == all_pieces;
  }

public:
  unsigned
  wk_can_castle() const noexcept
  { return wk_castle and can_castle<Color::White, BoardSide::King>(); }

  unsigned
  wq_can_castle() const noexcept
  { return wq_castle and can_castle<Color::White, BoardSide::Queen>(); }

  unsigned
  bk_can_castle() const noexcept
  { return bk_castle and can_castle<Color::Black, BoardSide::King>(); }

  unsigned
  bq_can_castle() const noexcept
  { return bq_castle and can_castle<Color::Black, BoardSide::Queen>(); }

  // Registers the Magics so instances of Board can generate moves.
  static void
  register_magics(
      std::unique_ptr<Magics> bmagics,
      std::unique_ptr<Magics> rmagics)
  {
    assert(bmagics != nullptr);
    assert(rmagics != nullptr);
    Board::bmagics = std::move(bmagics);
    Board::rmagics = std::move(rmagics);
  }

  //-----------------
  // Move generation.
  //-----------------

  MoveVec
  moves() const;

  MoveVec
  king_moves() const
  {
    MoveVec moves;
    king_moves(moves);
    return moves;
  }

  MoveVec
  knight_moves() const
  { return get_simple_moves(Piece::knight(), move_knight); }

  MoveVec
  bishop_moves() const
  {
    MoveVec moves;
    bishop_moves(moves);
    return moves;
  }

  MoveVec
  rook_moves() const
  {
    MoveVec moves;
    rook_moves(moves);
    return moves;
  }

  MoveVec
  queen_moves() const
  {
    MoveVec moves;
    queen_moves(moves);
    return moves;
  }

  MoveVec
  pawn_moves() const
  {
    MoveVec moves;
    pawn_moves(moves);
    return moves;
  }

private:
  //-------------------------------------
  // Private helpers for move generation.
  //-------------------------------------

  void
  king_moves(MoveVec& moves) const;

  void
  knight_moves(MoveVec& moves) const
  { return get_simple_moves(Piece::knight(), move_knight, moves); }

  void
  bishop_moves(MoveVec& moves) const;

  void
  rook_moves(MoveVec& moves) const;

  void
  queen_moves(MoveVec& moves) const;

  void
  pawn_moves(MoveVec& moves) const;

  // Computes simples moves for Bishops, Kights, Rooks, and Queens. Simple moves
  // consists of non-attack moves and attacks. |piece| is the piece moving, and
  // |moves_fn| is a function to compute moves for the given piece, including
  // non-attacks and attacks.
  void
  get_simple_moves(
      Piece piece,
      const std::function<BitBoard(BitBoard)>& moves_fn,
      MoveVec& moves) const;

  // Overload for get_simple_moves above, moves is returned instead of being
  // an output argument.
  MoveVec
  get_simple_moves(
      Piece piece,
      const std::function<BitBoard(BitBoard)>& moves_fn) const
  {
    MoveVec moves;
    get_simple_moves(piece, std::move(moves_fn), moves);
    return moves;
  }

  // Helper function to compute en passant moves.
  void
  move_enpassant(MoveVec& moves) const;

  // Computes all the empty squares that are attacked by other.
  Board& set_attacked() noexcept;

  friend BoardBuilder;

  // Note that we use static members for bmagics and rmagics below to avoid
  // using global variables, and to avoid having to pass these in as function
  // arguments. Move generation is best done from Board because moves are
  // tightly coupled to the state of a board.

  // For computing bishop magics.
  inline constinit static std::unique_ptr<Magics> bmagics = nullptr;

  // For computing rook magics.
  inline constinit static std::unique_ptr<Magics> rmagics = nullptr;

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

  // A bitboard mask to indicate which squares are attacked by the oppentent,
  // i.e. the other player not moving next. The purpose of this is to help
  // detect checks and move legality, e.g. king cannot castle if it has to go
  // through a check.
  BitBoard bb_attacked;

  std::uint16_t half_move = 0;
  std::uint16_t full_move = 0;

  // The next color to move.
  Color next;

  // If set, indicates that en passant is possible.
  bool en_passant = false;

  // Indicates the file where en passant is possible.
  bool en_passant_file = false;

  // Indicates if white can castle on kingside.
  bool wk_castle = false;

  // Indicates if white can castle on queenside.
  bool wq_castle = false;

  // Indicates if black can castle on kingside.
  bool bk_castle = false;

  // Indicates if black can castle on queenside.
  bool bq_castle = false;

  // Indicates if player moving next is in check.
  bool in_check = false;
};

inline bool
operator==(const Board& left, const Board& right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, const Board& board)
{ return os << board.str(); }

enum class BoardBuilderErr {
  // White pieces are not valid.
  White,
  // Black pieces are not valid.
  Black,
  // Too many half moves.
  HalfMove,
  // En passant file is not valid.
  EnPassantFile
};

class BoardBuilder {
public:
  std::expected<Board, BoardBuilderErr>
  build() const noexcept
  {
    if (!board.white().is_valid())
      return std::unexpected(BoardBuilderErr::White);

    if (!board.black().is_valid())
      return std::unexpected(BoardBuilderErr::White);

    if (file_err)
      return std::unexpected(BoardBuilderErr::EnPassantFile);

    if (half_move_err)
      return std::unexpected(BoardBuilderErr::HalfMove);

    return board;
  }

  // All pieces are set simulateously to make sure that we set mine and other
  // correctly with respect to the color to move next.
  BoardBuilder&
  set_pieces(Color color, const PieceSet& white, const PieceSet& black) noexcept
  {
    if (color == Color::White) {
      board.bb_mine = white;
      board.bb_other = black;
    } else {
      board.bb_mine = black;
      board.bb_other = white;
    }
    board.next = color;
    return *this;
  }

  BoardBuilder&
  set_half_move(unsigned hm) noexcept
  {
    if (hm > 100) {
      half_move_err = true;
      return *this;
    }

    half_move_err = false;
    board.half_move = hm;
    return *this;
  }

  BoardBuilder&
  set_full_move(unsigned fm) noexcept
  {
    board.full_move = fm;
    return *this;
  }

  BoardBuilder&
  set_enpassant_file(unsigned file) noexcept
  {
    if (file >= 8) {
      file_err = true;
      return *this;
    }

    file_err = false;
    board.en_passant = true;
    board.en_passant_file = file;

    return *this;
  }

  BoardBuilder&
  set_wk_castling(bool has_right) noexcept
  {
    board.wk_castle = has_right;
    return *this;
  }

  BoardBuilder&
  set_wq_castling(bool has_right) noexcept
  {
    board.wq_castle = has_right;
    return *this;
  }

  BoardBuilder&
  set_bk_castling(bool has_right) noexcept
  {
    board.bk_castle = has_right;
    return *this;
  }

  BoardBuilder&
  set_bq_castling(bool has_right) noexcept
  {
    board.bq_castle = has_right;
    return *this;
  }

private:
  Board board;
  bool file_err = false;
  bool half_move_err = false;
};

} // namespace blunder
