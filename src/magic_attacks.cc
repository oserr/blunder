#include "magic_attacks.h"

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

#include "par.h"

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

using ExpectedMagicResult = std::expected<std::pair<Magic, std::uint32_t>, Err>;

ExpectedMagicResult
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

std::expected<MagicAttacks, Err>
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

    if (not magic_info)
      return std::unexpected(magic_info.error());

    magics.push_back(std::move(magic_info->first));
  }

  return MagicAttacks(std::move(magics));
}

// Parallel version of find_all_magics.
std::expected<MagicAttacks, Err>
find_all_magics_par(
    par::WorkQ& workq,
    std::function<ExpectedMagicResult(std::uint64_t)> find_magic_fn)
{
  auto futs = workq.for_range(64, std::move(find_magic_fn));

  // Wait for all the futures to be ready.
  for (auto& fut : futs)
    fut.wait();

  // Now collect the resuts.
  std::vector<Magic> magics;
  magics.reserve(64);

  for (auto& fut : futs) {
    auto magic_info = fut.get();

    if (not magic_info)
      return std::unexpected(magic_info.error());

    magics.emplace_back(std::move(magic_info->first));
  }

  return MagicAttacks(std::move(magics));
}

std::function<std::uint64_t()>
create_rand_fn() noexcept
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
  auto magic_fn = create_rand_fn();
  return find_all_magics(get_bmask, get_battacks, magic_fn);
}

std::expected<MagicAttacks, Err>
compute_rmagics()
{
  auto magic_fn = create_rand_fn();
  return find_all_magics(get_rmask, get_rattacks, magic_fn);
}

std::expected<MagicAttacks, Err>
from_bmagics(std::span<const std::uint64_t> magics)
{
  assert(magics.size() == 64);
  auto magic_fn = [sq=0, magics=magics] mutable { return magics[sq++]; };
  return find_all_magics(get_bmask, get_battacks, magic_fn, 1);
}

std::expected<MagicAttacks, Err>
from_rmagics(std::span<const std::uint64_t> magics)
{
  auto magic_fn = [sq=0, magics=magics] mutable { return magics[sq++]; };
  return find_all_magics(get_rmask, get_rattacks, magic_fn, 1);
}

std::expected<MagicAttacks, Err>
SimpleMagicComputer::compute_bmagics() const
{ return compute_bmagics(); }

std::expected<MagicAttacks, Err>
SimpleMagicComputer::compute_rmagics() const
{ return compute_rmagics(); }

std::expected<MagicAttacks, Err>
SimpleMagicComputer::from_bmagics(std::span<const std::uint64_t> magics) const
{ return from_bmagics(magics); }

std::expected<MagicAttacks, Err>
SimpleMagicComputer::from_rmagics(std::span<const std::uint64_t> magics) const
{ return from_rmagics(magics); }

std::expected<MagicAttacks, Err>
ParMagicComputer::compute_bmagics() const
{
  auto fn = [
    mask_fn=get_bmask,
    attacks_fn=get_battacks,
    magic_fn=create_rand_fn()
  ](auto square) -> ExpectedMagicResult
  {
    return find_magic(square, mask_fn, attacks_fn, magic_fn);
  };

  return find_all_magics_par(*workq, std::move(fn));
}

std::expected<MagicAttacks, Err>
ParMagicComputer::compute_rmagics() const
{
  auto fn = [
    mask_fn=get_rmask,
    attacks_fn=get_rattacks,
    magic_fn=create_rand_fn()
  ](auto square) -> ExpectedMagicResult
  {
    return find_magic(square, mask_fn, attacks_fn, magic_fn);
  };

  return find_all_magics_par(*workq, std::move(fn));
}

std::expected<MagicAttacks, Err>
ParMagicComputer::from_bmagics(std::span<const std::uint64_t> magics) const
{
  assert(magics.size() == 64);

  auto fn = [
    mask_fn=get_bmask,
    attacks_fn=get_battacks,
    magics=magics
  ](auto square) -> ExpectedMagicResult
  {
    const auto m = magics[square];
    auto magic_fn = [m=m]() -> std::uint64_t { return m; };
    return find_magic(square, mask_fn, attacks_fn, magic_fn, 1);
  };

  return find_all_magics_par(*workq, std::move(fn));
}

std::expected<MagicAttacks, Err>
ParMagicComputer::from_rmagics(std::span<const std::uint64_t> magics) const
{
  assert(magics.size() == 64);

  auto fn = [
    mask_fn=get_rmask,
    attacks_fn=get_rattacks,
    magics=magics
  ](auto square) -> ExpectedMagicResult
  {
    const auto m = magics[square];
    auto magic_fn = [m=m]() -> std::uint64_t { return m; };
    return find_magic(square, mask_fn, attacks_fn, magic_fn, 1);
  };

  return find_all_magics_par(*workq, std::move(fn));
}

} // namespace blunder
