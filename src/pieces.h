#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

#include "bitboard.h"

namespace blunder {

enum class Piece : std::uint8_t {
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn,
  None
};

inline constexpr std::uint8_t
Uint8(Piece piece)
{
  return static_cast<std::uint8_t>(piece);
}


template<typename T>
requires std::is_integral_v<T>
constexpr Piece
ToPiece(T val)
{
  return static_cast<Piece>(val & 0b111);
}

std::uint8_t
GetPieceUint8(const std::array<BitBoard, 6> pieces, BitBoard piece) noexcept;

} // namespace blunder
