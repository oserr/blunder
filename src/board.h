#pragma once

#include <cassert>
#include <cstdint>
#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <utility>

#include "bitboard.h"
#include "board_side.h"
#include "color.h"
#include "game_state.h"
#include "magics.h"
#include "move.h"
#include "moves.h"
#include "pieces.h"
#include "piece_set.h"

namespace blunder {

// Forward declaration because we will make this a friend of Board to
// make it easier to build Board while preserving invariants.
class BoardBuilder;

// A structure to represent all squares that are attacked, including squres with
// and without pieces.
struct AttackSquares {
  BitBoard pieces;
  BitBoard no_pieces;
};

// Board represents the current state of the board. Some of the fields are
// written from the perspective of the player moving next to simplify move
// generation.
class Board {
public:
  // Returns a const reference to piece set for player moving next.
  const PieceSet&
  mine() const noexcept
  { return bb_mine; }

private:
  // Returns a mutable reference to piece set for player moving next.
  PieceSet&
  mine_mut() noexcept
  { return bb_mine; }

public:
  // Returns a reference to piece set for player not moving next.
  const PieceSet&
  other() const noexcept
  { return bb_other; }

private:
  // Returns a reference to piece set for player not moving next.
  PieceSet&
  other_mut() noexcept
  { return bb_other; }

public:
  // Returns true if white is next to move.
  bool
  is_white_next() const noexcept
  { return next_to_move == Color::White; }

  // Returns a pair of pointers to (white pieces, black pieces).
  std::pair<const PieceSet*, const PieceSet*>
  white_black() const noexcept
  {
    return is_white_next()
      ? std::make_pair(&bb_mine, &bb_other)
      : std::make_pair(&bb_other, &bb_mine);
  }

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

  // Returns true if we have reached a terminal state, i.e. we have a checkmate
  // or the game has been drawn.
  bool
  is_terminal() const noexcept
  {
    switch (game_state) {
      case GameState::Playing:
        [[fallthrough]];
      case GameState::Check:
        return false;
      default: break;
    }
    // We have a check mate or draw.
    return true;
  }

  // Returns true if game state is mate.
  bool
  is_mate() const noexcept
  { return game_state == GameState::Mate; }

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
      bits.set_bits(0b10010000ull);
      mask.set_bits(0b11110000ull);
    } else {
      bits.set_bits(0b00010001ull);
      mask.set_bits(0b00011111ull);
    }

    // Here we treat attacked squares as if they were occupied.
    auto all_pieces = all_bits() | other_attacks.no_pieces;

    if constexpr (color == Color::Black)
      all_pieces >>= 56;

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

  // Registers the precomputed Magics.
  static void
  register_magics();

  //-----------------
  // Move generation.
  //-----------------

  // Returns the next board states for all possible legal moves.
  std::vector<Board>
  next() const;

  MoveVec
  all_moves() const;

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

  // Returns the last move leading up to the current position, of one is
  // present. The scenarios where one might not be present are at the beginning
  // of a brand new game, or if we start a game mid way, such as from a FEN
  // string, which do not contain the game history.
  std::optional<Move>
  last_move() const noexcept
  {
    return prev_moves.empty()
      ? std::nullopt
      : std::make_optional(prev_moves.back());
  }

  // Updates this board with move |mv| if the move is pseudo-legal. Returns
  // true if the update succeeds, false otherwise.
  bool
  update_with_move(Move mv) noexcept;

  // Updates this board with a sequence of moves. Returns an exception the full
  // sequence of moves cannot be applied.
  Board&
  update_with_moves(std::span<const Move> moves);

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

  // If there is an opportunity for capture by en passant, then it returns the
  // file where the pawn can be captured. This is computed when we are updating
  // the state of the game after a move is made.
  std::optional<unsigned>
  compute_passant_file(Move mv) const noexcept;

  // Computes the game status.
  void
  compute_game_state() noexcept;

  // Same as above, but takes parameter |is_king_captured|, which can be used to
  // quickly check if the game is over when we compute the game status after a
  // move.
  void
  compute_game_state(bool is_king_captured) noexcept;

  // Updates this board with move |mv| without updating the game state.
  Board&
  quick_update(Move mv) noexcept;

  // Updates this board with move |mv|.
  Board&
  update(Move mv) noexcept;

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

  //------------------------------------------------------------------------
  // Utilities to compute BitBoards of all squares attacked by both players.
  //------------------------------------------------------------------------

  // Given a BitBoard |bb|, returns a BitBoard representing the set of bits in
  // |bb| that are attacked by all other pieces. For example, |bb| can be
  // the set of empty squares, or the set of all pieces for either current
  // player to move or the other player.
  BitBoard
  get_attacks(
      const PieceSet& pieces,
      BitBoard bb,
      bool is_white) const noexcept;

  // Returns the set of all squares attacked by other.
  AttackSquares
  get_attacks_other() const noexcept;

  // Computes all the empty squares that are attacked by other.
  Board&
  set_attacked_by_other() noexcept;

  // Returns the set of all squares attacked by mine.
  AttackSquares
  get_attacks_mine() const noexcept;

  // Computes all the empty squares that are attacked by other.
  Board&
  set_attacked_by_mine() noexcept;

  // Helper function to compute en passant moves.
  void
  move_enpassant(MoveVec& moves) const;

  // Checks if there is enough material for a win from either player. Returns
  // false for one of the following scenarios:
  // - king vs king
  // - king + knight vs king
  // - king + bishop vs king
  bool
  enough_material() const noexcept;

  // Returns true if the player moving next is in check.
  bool
  is_check() const noexcept
  {
    assert(mine().king().count() == 1);
    return mine().king() & other_attacks.pieces;
  }

  // Returns true if the other player is in check.
  bool
  is_check_other() const noexcept
  {
    assert(other().king().count() == 1);
    return other().king() & mine_attacks.pieces;
  }

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

  // Previous moves. Most recent move is last move leading up to current position.
  // TODO: consider creating a short ring buffer to only hold a few moves, e.g.
  // enough to be able to detect 3 repetitions.
  MoveVec prev_moves;

  // Members to hold all of the squares that are attacked by each player.
  // |mine_attacks| are squares attacked by the player moving next, and
  // |other_attacks| are squares attacked by the other player.
  AttackSquares mine_attacks;
  AttackSquares other_attacks;

  std::uint16_t half_move = 0;
  std::uint16_t full_move = 0;

  // The next color to move.
  Color next_to_move;

  // Indicates the file where en passant is possible.
  std::uint8_t en_passant_file = 0;

  // If set, indicates that en passant is possible.
  bool en_passant = false;

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

  // Indicates the state of the current position, e.g. the position represents
  // check mate.
  GameState game_state = GameState::Playing;
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
  build() noexcept
  {
    if (!board.white().is_valid())
      return std::unexpected(BoardBuilderErr::White);

    if (!board.black().is_valid())
      return std::unexpected(BoardBuilderErr::White);

    if (file_err)
      return std::unexpected(BoardBuilderErr::EnPassantFile);

    if (half_move_err)
      return std::unexpected(BoardBuilderErr::HalfMove);

    board.set_attacked_by_mine()
         .set_attacked_by_other()
         .compute_game_state();

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
    board.next_to_move = color;
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
