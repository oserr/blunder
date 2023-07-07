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
#include <set>
#include <utility>

#include "bitboard.h"
#include "err.h"

namespace blunder {
namespace {

inline constexpr std::uint32_t
get_magic_hash(
    BitBoard blocking,
    std::uint64_t magic,
    std::uint8_t magic_bits) noexcept
{
  return (blocking.raw() * magic) >> (64 - magic_bits);
}

std::expected<std::pair<Magic, std::uint32_t>, Err>
find_magic(
    std::uint32_t sq,
    std::function<BitBoard(std::uint32_t)> mask_fn,
    std::function<BitBoard(std::uint32_t, BitBoard)> attacks_fn,
    const std::function<std::uint64_t()>& magic_fn,
    std::uint32_t loops = 1000000000)
{
  BitBoard mask = mask_fn(sq);
  std::uint32_t num_bits = mask.count();

  if (num_bits < 5 or num_bits > 12)
    return std::unexpected(Err::MagicBitsOutOfRange);

  const auto ncombos = 1u << num_bits;
  constexpr unsigned kMaxCombos = 1u << 12;
  assert(ncombos <= kMaxCombos);

  BitBoard blocking[kMaxCombos];
  BitBoard attacks[kMaxCombos];
  for (auto i = 0u; i < ncombos; ++i) {
    blocking[i] = permute_mask(i, num_bits, mask);
    attacks[i] = attacks_fn(sq, blocking[i]);
  }

  std::vector<BitBoard> attack_table(ncombos);
  for (auto k = 0u; k < loops; ++k) {
    const auto magic = magic_fn();

    const auto num_high_bits = std::popcount((mask.raw()*magic) >> 56);
    if (num_high_bits < 6)
      continue;

    bool found_collision = false;
    for (auto i = 0u; i < ncombos; ++i) {
      auto magic_hash = get_magic_hash(blocking[i], magic, num_bits);

      if (not attack_table[magic_hash])
        attack_table[magic_hash] = attacks[i];
      else if (attack_table[magic_hash] != attacks[i]) {
        found_collision = true;
        break;
      }
    }

    // Try again if we found a collision.
    if (found_collision) {
      // Clear the attack table.
      std::ranges::for_each(attack_table, &BitBoard::clear);
      continue;
    }

    return std::make_pair(
        Magic(std::move(attack_table), mask, magic, num_bits), k);
  }

  // Unable to find a magic number.
  return std::unexpected(Err::MagicNotFound);
}

std::expected<std::vector<Magic>, Err>
find_all_magics(
    std::function<BitBoard(std::uint32_t)> mask_fn,
    std::function<BitBoard(std::uint32_t, BitBoard)> attacks_fn,
    const std::function<std::uint64_t()>& magic_fn,
    std::uint32_t loops = 1000000000)
{
  std::vector<Magic> magics;
  magics.reserve(64);

  for (auto s = 0u; s < 64u; ++s) {
    auto magic_info = find_magic(s, mask_fn, attacks_fn, magic_fn, loops);

    if (not magic_info) {
      std::cerr << "Unable to find magic for square=" << s << std::endl;
      return std::unexpected(magic_info.error());
    }

    std::cout << std::format("Computed magic={} for square={} in loops={}\n",
                              magic_info->first.magic, s, magic_info->second);

    magics.push_back(std::move(magic_info->first));
  }

  return magics;
}

std::function<std::uint64_t()>
CreateRandFn() noexcept
{
  std::random_device rand_dev;
  std::mt19937_64 rand_gen{rand_dev()};
  return [rand_gen] mutable { return rand_gen() & rand_gen() & rand_gen(); };
}

} // namespace

BitBoard
permute_mask(std::uint32_t num, std::uint32_t num_bits, BitBoard mask) noexcept
{
  BitBoard mask_combo;
  for (std::uint32_t i = 0; i < num_bits; ++i) {
    if (num & (1 << i))
      mask_combo.set_bit(mask.first_bit());
    mask.clear_first();
  }
  return mask_combo;
}

BitBoard
MagicAttacks::get_attacks(std::uint8_t square, BitBoard blockers) const noexcept
{
  assert(square < 64);
  assert(magics_.size() == 64);
  const auto& [attacks, mask, magic, nbits] = magics_[square];
  auto magic_hash = get_magic_hash(blockers & mask, magic, nbits);
  assert(magic_hash < attacks.size());

  return attacks[magic_hash];
}

std::expected<MagicAttacks, Err>
compute_bmagics()
{
  auto magic_fn = CreateRandFn();
  auto magics = find_all_magics(GetBishopMask, GetBishopAttacks, magic_fn);
  if (not magics)
    return std::unexpected(magics.error());
  return MagicAttacks(std::move(*magics));
}

std::expected<MagicAttacks, Err>
compute_rmagics()
{
  auto magic_fn = CreateRandFn();
  auto magics = find_all_magics(GetRookMask, GetRookAttacks, magic_fn);
  if (not magics)
    return std::unexpected(magics.error());
  return MagicAttacks(std::move(*magics));
}

std::expected<MagicAttacks, Err>
from_bmagics(std::span<const std::uint64_t> magics)
{
  assert(magics.size() == 64);
  auto magic_fn = [sq=0, magics=magics] mutable { return magics[sq++]; };
  auto all_magics = find_all_magics(
      GetBishopMask, GetBishopAttacks, magic_fn, 1);
  if (not all_magics)
    return std::unexpected(all_magics.error());
  return MagicAttacks(std::move(*all_magics));
}

std::expected<MagicAttacks, Err>
from_rmagics(std::span<const std::uint64_t> magics)
{
  auto magic_fn = [sq=0, magics=magics] mutable { return magics[sq++]; };
  auto all_magics = find_all_magics(GetRookMask, GetRookAttacks, magic_fn, 1);
  if (not all_magics)
    return std::unexpected(all_magics.error());
  return MagicAttacks(std::move(*all_magics));
}

} // namespace blunder
