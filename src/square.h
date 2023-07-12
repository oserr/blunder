#pragma once

#include <initializer_list>
#include <set>
#include <string>

#include "bitboard.h"

namespace blunder {

// Represents the squres on a chess board.
enum class Sq {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,
};

using SqList = std::initializer_list<Sq>;

inline unsigned
to_int(Sq sq) noexcept
{ return static_cast<unsigned>(sq); }

inline Sq
to_sq(unsigned val) noexcept
{
  assert(val < 64);
  return static_cast<Sq>(val);
}

// Converts a Sq to a BitBoard.
inline BitBoard
to_bitboard(Sq sq) noexcept
{ return BitBoard::from_index(to_int(sq)); }

std::string
to_sq_str(unsigned val);

inline std::string
str(Sq sq)
{ return to_sq_str(to_int(sq)); }

std::set<Sq>
to_set_of_sq(BitBoard bb);

BitBoard
to_bitboard(SqList squares) noexcept;

std::string
to_list_str(const std::set<Sq>& squares);

std::string
to_list_str(SqList squares);

} // namespace blunder
