#pragma once

#include "bitboard.h"

namespace blunder {

constexpr BitBoard
move_north(BitBoard bb) noexcept
{ return bb << 8; }

constexpr BitBoard
move_south(BitBoard bb) noexcept
{ return bb >> 8; }

constexpr BitBoard
move_king(BitBoard king) noexcept
{
  auto k = king;
  const auto left = (k & ~kFileA) >> 1;
  const auto right = (k & ~kFileH) << 1;
  k |= left | right;
  const auto down = (k & ~kRank1) >> 8;
  const auto up = (k & ~kRank8) << 8;
  return (k | down | up) & ~king;
}

// Returns the set of squares where a king can move, except for castling.
constexpr BitBoard
move_knight(BitBoard knight) noexcept
{
  // One square up, two right.
  const auto bits1 = (knight & ~kFileG & ~kFileH) << 10;
  // Two square up, one right.
  const auto bits2 = (knight & ~kFileH) << 17;
  // Two square up, one left.
  const auto bits3 = (knight & ~kFileA) << 15;
  // One square up, two left.
  const auto bits4 = (knight & ~kFileA & ~kFileB) << 6;
  // One square down, two left.
  const auto bits5 = (knight & ~kFileA & ~kFileB) >> 10;
  // Two square down, one left.
  const auto bits6 = (knight & ~kFileA) >> 17;
  // Two square down, one right.
  const auto bits7 = (knight & ~kFileH) >> 15;
  // One square down, two right.
  const auto bits8 = (knight & ~kFileG & ~kFileH) >> 6;

  return bits1 | bits2 | bits3 | bits4
       | bits5 | bits6 | bits7 | bits8;
}

// Returns the set of white pawns after moving one square forward.
constexpr BitBoard
move_wp_single(BitBoard pawns, BitBoard empty) noexcept
{ return move_north(pawns) & empty; }

// Returns the set of white pawns afer moving two squares forward for pawns that
// can move two squares forward.
constexpr BitBoard
move_wp_double(BitBoard pawns, BitBoard empty) noexcept
{
  pawns = move_wp_single(pawns, empty);
  return move_wp_single(pawns, empty) & kRank4;
}

// Returns the set of white pawns after attacking diagonal left.
constexpr BitBoard
move_wp_left(BitBoard pawns, BitBoard all_black) noexcept
{ return pawns << 7 & all_black & ~kFileH; }

// Returns the set of white pawns after attacking diagonal right.
constexpr BitBoard
move_wp_right(BitBoard pawns, BitBoard all_black) noexcept
{ return pawns << 9 & all_black & ~kFileA; }

// Returns the set of black pawns after moving one square forward.
constexpr BitBoard
move_bp_single(BitBoard pawns, BitBoard empty) noexcept
{ return move_south(pawns) & empty; }

// Returns the set of black pawns after moving two squares forward for pawns
// that can move two squares forward.
constexpr BitBoard
move_bp_double(BitBoard pawns, BitBoard empty) noexcept
{
  pawns = move_bp_single(pawns, empty);
  return move_bp_single(pawns, empty) & kRank5;
}

// Returns the set of black pawns after attacking diagonal left.
constexpr BitBoard
move_bp_left(BitBoard pawns, BitBoard all_white) noexcept
{ return pawns >> 7 & all_white & ~kFileA; }

// Returns the set of black pawns after attacking diagonal right.
constexpr BitBoard
move_bp_right(BitBoard pawns, BitBoard all_white) noexcept
{ return pawns >> 9 & all_white & ~kFileH; }

}
