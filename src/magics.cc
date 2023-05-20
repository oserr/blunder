#include "magics.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <random>
#include <utility>

#include "bitboard.h"
#include "error.h"

namespace blunder {
namespace {

constexpr std::array<int, 64> kRookShifts = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

constexpr std::array<int, 64> kBishopShifts = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

constexpr int
DoMagic(BitBoard blocking, std::uint64_t magic, int magic_bits) noexcept
{
  return (blocking * magic) >> (64 - magic_bits);
}

// Computes a subset of set bits in |mask| based on |num|. This is done by using
// the first |num_bits| in |num| to determine to set the bits in the subset of
// |mask|. For example, if we have the following inputs
//
// - num = 0b00000100
// - num_bits = 5
// - mask = 0b0111000
//
// then PremuteMask returns 0b0100000, because only the 2nd (0-based index) bit
// is set in |num|, and hence only the 2nd set bit in mask is set.
BitBoard
PermuteMask(std::uint32_t num, std::uint32_t num_bits, BitBoard mask) noexcept
{
  BitBoard mask_combo = 0;
  for (std::uint32_t i = 0; i < num_bits; i++) {
    if (num & (1 << i))
      mask_combo |= 1ull << std::countr_zero(mask);
    mask &= mask - 1;
  }
  return mask_combo;
}

std::expected<std::pair<Magic, std::uint32_t>, Error>
FindMagic(
    std::uint32_t sq,
    std::uint32_t magic_bits,
    std::function<BitBoard(std::uint32_t)> mask_fn,
    std::function<BitBoard(std::uint32_t, BitBoard)> attacks_fn,
    std::function<std::uint64_t()> magic_fn,
    std::uint32_t loops = 1000000000) {
  BitBoard mask = mask_fn(sq);
  std::uint32_t num_bits = std::popcount(mask);

  if (num_bits != magic_bits) {
    std::cerr << "magic_bits("
              << magic_bits
              << ") != num_bits("
              << num_bits << ")"
              << std::endl;
  }

  if (num_bits < 5 or num_bits > 12)
    return std::unexpected(Error::MagicBitsOutOfRange);

  const auto ncombos = 1u << num_bits;
  constexpr unsigned kMaxCombos = 1 << 12;
  assert(ncombos <= kMaxCombos);

  BitBoard blocking[kMaxCombos];
  BitBoard attacks[kMaxCombos];
  for (auto i = 0u; i < ncombos; ++i) {
    blocking[i] = PermuteMask(i, num_bits, mask);
    attacks[i] = attacks_fn(sq, blocking[i]);
  }

  std::vector<BitBoard> attack_table(ncombos);
  for (auto k = 0u; k < loops; ++k) {
    const auto magic = magic_fn();

    const auto num_high_bits = std::popcount((mask*magic) >> 56);
    if (num_high_bits < 6) continue;

    // Clear the attack table.
    std::ranges::fill(attack_table, 0);

    bool found_collision = false;
    for (auto i = 0u; i < ncombos; ++i) {
      auto magic_hash = DoMagic(blocking[i], magic, num_bits);

      if (not attack_table[magic_hash])
        attack_table[magic_hash] = attacks[i];
      else if (attack_table[magic_hash] != attacks[i]) {
        found_collision = true;
        break;
      }
    }

    // Try again if we found a collision.
    if (found_collision) continue;

    return std::make_pair(Magic(std::move(attack_table), magic, num_bits), k);
  }

  // Unable to find a magic number.
  return std::unexpected(Error::MagicNotFound);
}

} // namespace

std::expected<MagicAttacks, Error>
MagicAttacks::ComputeBishopMagics()
{
  std::vector<Magic> magics;
  magics.reserve(64);

  std::random_device rand_dev;
  std::mt19937_64 rand_gen{rand_dev()};
  auto rand_fn = [&rand_gen]{ return rand_gen() & rand_gen() & rand_gen(); };

  for (auto s = 0u; s < 64u; ++s) {
    auto magic_info =
      FindMagic(s, kBishopShifts[s], GetBishopMask, GetBishopAttacks, rand_fn);

    if (not magic_info) {
      std::cerr << "Unable to find magic for square=" << s << std::endl;
      return std::unexpected(magic_info.error());
    }

    std::cout << std::format("Computed magic={} for square={} in loops={}\n",
                              magic_info->first.magic, s, magic_info->second);

    magics.push_back(std::move(magic_info->first));
  }

  return MagicAttacks(std::move(magics));
}

std::expected<MagicAttacks, Error>
MagicAttacks::ComputeRookMagics()
{
  std::vector<Magic> magics;
  magics.reserve(64);

  std::random_device rand_dev;
  std::mt19937_64 rand_gen{rand_dev()};
  auto rand_fn = [&rand_gen]{ return rand_gen() & rand_gen() & rand_gen(); };

  for (auto s = 0u; s < 64u; ++s) {
    auto magic_info =
      FindMagic(s, kRookShifts[s], GetRookMask, GetRookAttacks, rand_fn);

    if (not magic_info) {
      std::cerr << "Unable to find magic for square=" << s << std::endl;
      return std::unexpected(magic_info.error());
    }

    std::cout << std::format("Computed magic={} for square={} in loops={}\n",
                              magic_info->first.magic, s, magic_info->second);

    magics.push_back(std::move(magic_info->first));
  }

  return MagicAttacks(std::move(magics));
}

MagicAttacks
MagicAttacks::InitFromBishopMagics(std::span<std::uint64_t> magics)
{
  (void) magics;
  return MagicAttacks(std::vector<Magic>());
}

MagicAttacks
MagicAttacks::InitFromRookMagics(std::span<std::uint64_t> magics)
{
  (void) magics;
  return MagicAttacks(std::vector<Magic>());
}

BitBoard
MagicAttacks::GetAttacks(std::uint8_t square, BitBoard blockers) const noexcept
{
  (void) square;
  (void) blockers;
  return BitBoard();
}

std::span<const Magic>
MagicAttacks::GetMagics() const noexcept
{
  return std::span(magics_);
}

} // namespace blunder
