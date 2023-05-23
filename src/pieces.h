#pragma once

#include <cstdint>

namespace blunder {

enum class Piece : std::uint8_t {
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn
};

enum class Color : std::uint8_t {
  White,
  Black
};

} // namespace blunder
