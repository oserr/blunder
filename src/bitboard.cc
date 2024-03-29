#include "bitboard.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

namespace blunder {

// TODO: Determine if we can improve this by using SIMD instructions.
BitBoard&
BitBoard::flip_this() noexcept
{
  constexpr auto h1 = 0x5555555555555555ull;
  constexpr auto h2 = 0x3333333333333333ull;
  constexpr auto h4 = 0x0F0F0F0F0F0F0F0Full;
  constexpr auto v1 = 0x00FF00FF00FF00FFull;
  constexpr auto v2 = 0x0000FFFF0000FFFFull;
  bits = ((bits >> 1) & h1) | ((bits & h1) << 1);
  bits = ((bits >> 2) & h2) | ((bits & h2) << 2);
  bits = ((bits >> 4) & h4) | ((bits & h4) << 4);
  bits = ((bits >> 8) & v1) | ((bits & v1) << 8);
  bits = ((bits >> 16) & v2) | ((bits & v2) << 16);
  bits = (bits >> 32) | (bits << 32);
  return *this;
}

std::string
BitBoard::fancy_str() const
{
  // Reverse the bytes to start from rank 8.
  auto arr_bits = std::bit_cast<std::array<std::byte, sizeof(bits)>>(bits);
  std::ranges::reverse(arr_bits);
  auto rev_bits = std::bit_cast<std::uint64_t>(arr_bits);

  std::string buff;
  buff.reserve(1024);

  for (size_t i = 0; i < 8; ++i, rev_bits >>= 8) {
    buff +=  "+---+---+---+---+---+---+---+---+\n|";
    auto row_bits = rev_bits & 0xff;
    for (size_t j = 0; j < 8; ++j, row_bits >>= 1)
      buff += row_bits & 1 ? " 1 |" : "   |";
    buff += '\n';
  }

  buff +=  "+---+---+---+---+---+---+---+---+\n";

  return buff;
}

BitBoard
get_rattacks(std::uint32_t sq, BitBoard blocking) noexcept
{
  assert(sq < 64);

  BitBoard attacks;
  const int rk = sq / 8;
  const int fl = sq % 8;

  // Bitscan upwards towards 8th rank.
  for (int r = rk+1; r <= 7; ++r) {
    auto index = r*8 + fl;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan downwards towards 1st rank.
  for (int r = rk-1; r >= 0; --r) {
    auto index = r*8 + fl;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan east towards the H file.
  for (int f = fl+1; f <= 7; ++f) {
    auto index = rk*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan west towards the A file.
  for (int f = fl-1; f >= 0; --f) {
    auto index = rk*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  return attacks;
}

BitBoard
get_battacks(std::uint32_t sq, BitBoard blocking) noexcept
{
  assert(sq < 64);

  BitBoard attacks;
  const int rk = sq / 8;
  const int fl = sq % 8;

  // Bitscan northeast.
  for (int r = rk+1, f = fl+1; r <= 7 and f <= 7; ++r, ++f) {
    auto index = r*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan northwest.
  for (int r = rk+1, f = fl-1; r <= 7 and f >= 0; ++r, --f) {
    auto index = r*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan southeast.
  for (int r = rk-1, f = fl+1; r >= 0 and f <= 7; --r, ++f) {
    auto index = r*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  // Bitscan southwest.
  for (int r = rk-1, f = fl-1; r >= 0 and f >= 0; --r, --f) {
    auto index = r*8 + f;
    attacks.set_bit(index);
    if (blocking.is_set(index)) break;
  }

  return attacks;
}

} // namespace blunder
