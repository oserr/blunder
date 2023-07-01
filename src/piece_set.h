#pragma once

#include <array>
#include <cassert>
#include <cstdint>

#include "bitboard.h"
#include "pieces.h"

namespace blunder {

class PieceSet {
public:
  //----------
  // Accessors
  //----------

  BitBoard
  get(Piece piece) const noexcept
  {
    assert(piece.type() != Type::None and "PieceSet does not contain None.");
    return pieces[piece.uint()];
  }

  BitBoard
  get(Type type) const noexcept
  { return get(Piece(type)); }

  BitBoard
  king() const noexcept
  { return get(Type::King); }

  BitBoard
  queen() const noexcept
  { return get(Type::Queen); }

  BitBoard
  rook() const noexcept
  { return get(Type::Rook); }

  BitBoard
  bishop() const noexcept
  { return get(Type::Bishop); }

  BitBoard
  knight() const noexcept
  { return get(Type::Knight); }

  BitBoard
  pawn() const noexcept
  { return get(Type::Pawn); }

  BitBoard
  full_set() const noexcept
  { return all_bits; }

  //----------
  // Setters
  //----------
  
  PieceSet&
  set_bit(Type type, unsigned index) noexcept
  { 
    get_mut(type).set_bit(index); 
    all_bits.set_bit(index);
    return *this;
  }

  PieceSet&
  set_bit(Piece piece, unsigned index) noexcept
  { return set_bit(piece.type(), index); }

  //----------
  // Iterators
  //----------

  auto
  begin() noexcept
  { return pieces.begin(); }

  auto
  end() noexcept
  { return pieces.end(); }

  auto
  begin() const noexcept
  { return pieces.cbegin(); }

  auto
  end() const noexcept
  { return pieces.cend(); }

  bool
  eq(const PieceSet& other) const noexcept
  { return pieces == other.pieces and all_bits == other.all_bits; }

  // Given a bitboard with one bit set, finds the piece type for the given
  // bitboard.
  Piece
  find_type(BitBoard bb) const noexcept;

  // Initializes a PieceSet with the full set of white pieces positioned for a
  // new game.
  static PieceSet
  init_white() noexcept;

  // Initializes a PiceSet with the full set of black pieces positioned for a
  // new game.
  static PieceSet
  init_black() noexcept;

private:

  BitBoard&
  get_mut(Type type) noexcept
  {
    assert(type != Type::None and "Cannot get None.");
    return pieces[uint(type)];
  }

  // BitBoards representing the pieces for one side.
  // 0 - king
  // 1 - queen
  // 2 - rook
  // 3 - bishop
  // 4 - knight
  // 5 - pawns
  std::array<BitBoard, 6> pieces;

  // The bitboard containing all of the pieces.
  BitBoard all_bits;
};

inline bool
operator==(const PieceSet& left, const PieceSet& right) noexcept
{ return left.eq(right); }

std::uint8_t
GetPieceUint8(const std::array<BitBoard, 6> pieces, BitBoard piece) noexcept;

} // namespace blunder
