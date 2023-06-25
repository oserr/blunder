#include "bitboard.h"

#include <cassert>
#include <cstdint>

namespace blunder {

BitBoard
GetRookAttacks(std::uint32_t sq, BitBoard blocking) noexcept
{
  assert(sq < 64);

  BitBoard attacks = 0;
  const int rk = sq / 8;
  const int fl = sq % 8;

  // Bitscan upwards towards 8th rank.
  for (int r = rk+1; r <= 7; ++r) {
    const BitBoard bit = 1ull << (r*8 + fl);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan downwards towards 1st rank.
  for (int r = rk-1; r >= 0; --r) {
    const BitBoard bit = 1ull << (r*8 + fl);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan east towards the H file.
  for (int f = fl+1; f <= 7; ++f) {
    const BitBoard bit = 1ull << (rk*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan west towards the A file.
  for (int f = fl-1; f >= 0; --f) {
    const BitBoard bit = 1ull << (rk*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  return attacks;
}

BitBoard
GetBishopAttacks(std::uint32_t sq, BitBoard blocking) noexcept
{
  assert(sq < 64);

  BitBoard attacks = 0;
  const int rk = sq / 8;
  const int fl = sq % 8;

  // Bitscan northeast.
  for (int r = rk+1, f = fl+1; r < 7 and f < 7; ++r, ++f) {
    const BitBoard bit = 1ull << (r*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan northwest.
  for (int r = rk+1, f = fl-1; r < 7 and f > 0; ++r, --f) {
    const BitBoard bit = 1ull << (r*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan southeast.
  for (int r = rk-1, f = fl+1; r > 0 and f < 7; --r, ++f) {
    const BitBoard bit = 1ull << (r*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  // Bitscan southwest.
  for (int r = rk-1, f = fl-1; r > 0 and f > 0; --r, --f) {
    const BitBoard bit = 1ull << (r*8 + f);
    attacks |= bit;
    if (bit & blocking) break;
  }

  return attacks;
}

} // namespace blunder
