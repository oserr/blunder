#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <optional>

#include "bitboard.h"
#include "color.h"
#include "pieces.h"

namespace blunder {

class PieceSet {
public:
  //----------
  // Accessors
  //----------

  BitBoard
  get(Piece piece) const;

  BitBoard
  get(Type type) const noexcept
  { return get(Piece(type)); }

  BitBoard
  king() const noexcept
  { return kings; }

  BitBoard
  queen() const noexcept
  { return queens; }

  BitBoard
  rook() const noexcept
  { return rooks; }

  BitBoard
  bishop() const noexcept
  { return bishops; }

  BitBoard
  knight() const noexcept
  { return knights; }

  BitBoard
  pawn() const noexcept
  { return pawns; }

  BitBoard
  all() const noexcept
  { return all_bits; }

  //----------
  // Setters
  //----------

  PieceSet&
  set_bit(Type type, unsigned index) noexcept;

  PieceSet&
  set_bit(Piece piece, unsigned index) noexcept
  { return set_bit(piece.type(), index); }

  PieceSet&
  clear_bit(Type type, unsigned index) noexcept;

  PieceSet&
  clear_bit(Piece piece, unsigned index) noexcept
  { return clear_bit(piece.type(), index); }

  PieceSet&
  update_bit(Type type, unsigned from, unsigned to);

  PieceSet&
  update_bit(Piece piece, unsigned from, unsigned to)
  { return update_bit(piece.type(), from, to); }

  // Flips all the pieces horizontally across the board to switch between
  // perspective for white or black.
  PieceSet
  flip() const noexcept;

  // Given a bitboard with one bit set, finds the piece type for the given
  // bitboard.
  std::optional<Piece>
  find_type(BitBoard bb) const noexcept;

  // Initializes a PieceSet with the full set of white pieces positioned for a
  // new game.
  static PieceSet
  init_white() noexcept;

  // Initializes a PiceSet with the full set of black pieces positioned for a
  // new game.
  static PieceSet
  init_black() noexcept;

  // Returns true if the piece set is valid, false otherwise. For exampe, the
  // set should have exactly one king, no more than 8 pawns, no more than 16
  // total pieces, etc.
  bool
  is_valid() const noexcept;

  // Default equality operators.
  friend bool operator==(const PieceSet&, const PieceSet&) = default;

private:
  BitBoard&
  get_mut(Type type) noexcept;

  BitBoard kings;
  BitBoard queens;
  BitBoard rooks;
  BitBoard bishops;
  BitBoard knights;
  BitBoard pawns;
  // Contains all the pieces.
  BitBoard all_bits;
  Color color;
  // True if the king has castling right.
  bool king_castle;
  // True if the queen has castling right.
  bool queen_castle;
};

} // namespace blunder
