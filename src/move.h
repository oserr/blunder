#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "pieces.h"
#include "square.h"

namespace blunder {

enum class MoveType : std::uint8_t {
  Normal,
  KingCastle,
  QueenCastle,
  EnPassant,
  Promo
};

// Move contains multiple bitfields to represent a chess move, taking less than
// 4 total bytes.
class Move {
public:
  // Initializes all fields.
  Move() = default;

  // ------------------------------------------------------------------------
  // Most common scenario for a move, i.e. we move a piece from one square to
  // another without attacking, and hence this initializes the move with the
  // source square, piece type for moving piece, and destination square.
  // ------------------------------------------------------------------------

  Move(Piece fp, std::uint8_t fs, std::uint8_t ts) noexcept
    : from_piece(fp),
      from_square(fs),
      to_square(ts) {}

  Move(Piece fp, Sq fs, Sq ts) noexcept
    : Move(fp, to_int(fs), to_int(ts)) {}

  // ------------------------------------------------------------------------
  // Next most common scenario, we move a piece with capture, with overloads.
  // ------------------------------------------------------------------------

  Move(Piece fp, std::uint8_t fs, Piece tp, std::uint8_t ts) noexcept
    : from_piece(fp),
      to_piece(tp),
      from_square(fs),
      to_square(ts) {}

  Move(Piece fp, Sq fs, Piece tp, Sq ts) noexcept
    : Move(fp, to_int(fs), tp, to_int(ts)) {}

  // Copy control.
  Move(const Move& m) noexcept = default;
  Move& operator=(const Move& m) noexcept = default;

  // --------------------------------------------------------------------------
  // Convenience functions to initialize special types of moves, like castling,
  // pawn promos, etc.
  // --------------------------------------------------------------------------

  // Returns a move for white king side castle.
  static Move
  wk_castle() noexcept
  {
    Move mv(Piece::king(), 4, 6);
    mv.move_type = MoveType::KingCastle;
    return mv;
  }

  // Returns a move for white queen side castle.
  static Move
  wq_castle() noexcept
  {
    Move mv(Piece::king(), 4, 2);
    mv.move_type = MoveType::QueenCastle;
    return mv;
  }

  // Returns a move for black king side castle.
  static constexpr Move
  bk_castle() noexcept
  {
    Move mv(Piece::king(), 60, 62);
    mv.move_type = MoveType::KingCastle;
    return mv;
  }

  // Returns a move for black queen side castle.
  static constexpr Move
  bq_castle() noexcept
  {
    Move mv(Piece::king(), 60, 58);
    mv.move_type = MoveType::QueenCastle;
    return mv;
  }

  // Returns a move for pawn promotion without capture.
  static Move
  promo(std::uint8_t fs, std::uint8_t ts, Piece promo) noexcept
  {
    assert(fs < 64);
    assert(ts < 64);

    Move mv(Piece::pawn(), fs, ts);
    mv.move_type = MoveType::Promo;
    mv.promo_piece = promo;
    return mv;
  }

  // Returns a move for pawn promotion without capture.
  static Move
  promo(Sq fs, Sq ts, Piece promo) noexcept
  {
    Move mv(Piece::pawn(), to_int(fs), to_int(ts));
    mv.move_type = MoveType::Promo;
    mv.promo_piece = promo;
    return mv;
  }

  // Returns a move for pawn promotion with capture.
  static Move
  promo(std::uint8_t fs, Piece tp, std::uint8_t ts, Piece promo) noexcept
  {
    Move mv(Piece::pawn(), fs, tp, ts);
    mv.move_type = MoveType::Promo;
    mv.promo_piece = promo;
    return mv;
  }

  // Returns a move for en passant capture.
  //
  // @param fs The source square where piece is moving from.
  // @param ts The destination square, where piece is moving to.
  // @param ts The en-passant squaure, where pawn is being captured.
  static Move
  by_enpassant(std::uint8_t fs, std::uint8_t ts, std::uint8_t ps) noexcept
  {
    assert(fs < 64);
    assert(ts < 64);

    Move mv(Piece::pawn(), fs, Piece::pawn(), ts);
    mv.passant_square = ps;
    mv.move_type = MoveType::EnPassant;
    return mv;
  }

  // Cretes a printable debug string for Move in the form
  //
  //   {P:from_square -> to_square, ...}
  //
  // where P is a color agnostic piece type, and ... may contain the rook move
  // in a castling move, a captured piece !P, and a promoted pawn piece ^P.
  std::string
  str() const;

  // Returns true if both moves are equal.
  bool
  eq(Move mv) const noexcept;

  //-----------
  // Accessors.
  //-----------

  Piece
  piece() const noexcept
  { return from_piece; }

  Piece
  capture() const noexcept
  { return to_piece; }

  unsigned
  from() const noexcept
  { return from_square; }

  unsigned
  to() const noexcept
  { return to_square; }

  MoveType
  type() const noexcept
  { return move_type; }

  Piece
  promoted() const noexcept
  { return promo_piece; }

  // Returns the square of the pawn being captured by en-passant.
  unsigned
  passant() const noexcept
  { return passant_square; }

  bool
  is_promo() const noexcept
  { return type() == MoveType::Promo; }

  bool
  is_enpassant() const noexcept
  { return type() == MoveType::EnPassant; }

  bool
  is_kcastling() const noexcept
  { return type() == MoveType::KingCastle; }

  bool
  is_qcastling() const noexcept
  { return type() == MoveType::QueenCastle; }

  bool
  is_castling() const noexcept
  { return is_kcastling() or is_qcastling(); }

  bool
  is_capture() const noexcept
  { return capture().type() != Type::None; }

  // If this is a castling move, returns a pair of (from, to) with source and
  // destination squares for the rook.
  std::optional<std::pair<unsigned, unsigned>>
  get_rook_from_to() const noexcept;

private:
  // The color of the piece moving is not encoded as a member because moves are
  // done in the context of a Board and a game, and the color can be determined
  // from the context.

  // Represents the piece being moved. If the move does not represent an attack,
  // then |capture| is set to none.
  Piece from_piece = Piece::none();
  Piece to_piece = Piece::none();

  // The source and destination squares.
  std::uint8_t from_square = 0;
  std::uint8_t to_square = 0;

  // Identifies special move types, e.g. castling, pawn promo, etc.
  MoveType move_type = MoveType::Normal;

  // The square of the captured piece on an en-passant move. Note that this can
  // be deduced from context, but include here to make it easier to apply the
  // moves.
  std::uint8_t passant_square = 0;

  // For pawn promotions, the promoted piece type.
  Piece promo_piece = Piece::none();

  // TODO: add one more member with a numeric ID to represent the move code
  // for use with the neural network.
};

inline bool
operator==(Move left, Move right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, Move mv)
{ return os << mv.str(); }

using MoveVec = std::vector<Move>;

} // namespace blunder
