#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <functional>
#include <ostream>

#include "bitboard.h"
#include "color.h"
#include "hash.h"

namespace blunder {

enum struct Type : std::uint8_t {
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn,
  None
};

inline unsigned
to_int(Type type) noexcept
{ return static_cast<unsigned>(type); }

class Piece {
public:
  Piece(Type type) noexcept
    : ptype(type) {}

  Piece() noexcept
    : ptype(Type::None) {}

  //-------------
  // Copy control.
  //-------------

  Piece(const Piece& piece) noexcept = default;
  Piece(Piece&& piece) noexcept = default;
  Piece& operator=(const Piece& piece) noexcept = default;
  Piece& operator=(Piece&& piece) noexcept = default;

  // Returns the piece type.
  Type
  type() const noexcept
  { return ptype; }

  // Returns the underlying integral value of the type.
  unsigned
  uint() const noexcept
  { return to_int(ptype); }

  // Initializes a Piece form an int-like type.
  template<typename T>
  requires std::integral<T>
  static Piece
  from_int(T val) noexcept
  { return Piece(static_cast<Type>(val)); }

  // Returns the ascii letter initial of the piece. Upercase for white and lower
  // case for black.
  std::uint8_t
  letter(Color color = Color::White) noexcept;

  // Returns true if other is the same piece type.
  bool
  eq(Piece other) const noexcept
  { return ptype == other.ptype; }

  //--------------------------------------------------------
  // Helpers to check if the piece is a specific piece type.
  //--------------------------------------------------------

  bool
  is_type(Type piece_type) const noexcept
  { return type() == piece_type; }

  bool
  is_king() const noexcept
  { return is_type(Type::King); }

  bool
  is_queen() const noexcept
  { return is_type(Type::Queen); }

  bool
  is_rook() const noexcept
  { return is_type(Type::Rook); }

  bool
  is_bishop() const noexcept
  { return is_type(Type::Bishop); }

  bool
  is_knight() const noexcept
  { return is_type(Type::Knight); }

  bool
  is_pawn() const noexcept
  { return is_type(Type::Pawn); }

  bool
  is_none() const noexcept
  { return is_type(Type::None); }

  // Computes the hash of this piece.
  size_t
  hsh() const noexcept
  { return combine_hash(to_int(ptype)); }

  //----------------------------------------------------
  // Static helpers to create pieces of different types.
  //----------------------------------------------------

  static Piece
  king() noexcept
  { return Piece(Type::King); }

  static Piece
  queen() noexcept
  { return Piece(Type::Queen); }

  static Piece
  rook() noexcept
  { return Piece(Type::Rook); }

  static Piece
  bishop() noexcept
  { return Piece(Type::Bishop); }

  static Piece
  knight() noexcept
  { return Piece(Type::Knight); }

  static Piece
  pawn() noexcept
  { return Piece(Type::Pawn); }

  static Piece
  none() noexcept
  { return Piece(Type::None); }

private:
  Type ptype;
};

inline bool
operator==(Piece left, Piece right) noexcept
{ return left.eq(right); }

inline std::ostream&
operator<<(std::ostream& os, Piece piece)
{ return os << piece.letter(); }

inline std::uint8_t
letter(Piece piece, Color color = Color::White) noexcept
{ return piece.letter(color); }

inline std::uint8_t
letter(Type type, Color color = Color::White) noexcept
{ return Piece(type).letter(color); }

} // namespace blunder

template<>
struct std::hash<blunder::Piece> {
  std::size_t
  operator()(blunder::Piece piece) const noexcept
  { return piece.hsh(); }
};
