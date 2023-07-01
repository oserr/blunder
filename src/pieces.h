#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <ostream>

#include "bitboard.h"
#include "color.h"

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
uint(Type type) noexcept
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
  { return static_cast<unsigned>(ptype); }

  // Initializes a Piece form an int-like type.
  template<typename T>
  requires std::integral<T>
  static Piece
  from_int(T val) noexcept
  { return static_cast<Type>(val & 0xb111); }

  // Returns the ascii letter initial of the piece. Upercase for white and lower
  // case for black.
  std::uint8_t
  letter(Color color = Color::White) noexcept;

  // Returns true if other is the same piece type.
  bool
  eq(Piece other) const noexcept
  { return ptype == other.ptype; }

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
