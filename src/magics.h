#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>
#include <vector>

#include "bitboard.h"
#include "err.h"

namespace blunder {

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
permute_mask(std::uint32_t num, std::uint32_t num_bits, BitBoard mask) noexcept;

struct Magic {
  std::vector<BitBoard> attacks;
  BitBoard mask;
  std::uint64_t magic = 0;
  std::uint8_t nbits = 0;
};

class MagicAttacks {
public:
  explicit MagicAttacks(
    std::vector<Magic> magics)
    : magics_(std::move(magics)) {}

  MagicAttacks(const MagicAttacks& magic_attacks) noexcept = default;
  MagicAttacks(MagicAttacks&& magic_attacks) noexcept = default;

  BitBoard
  GetAttacks(std::uint8_t square, BitBoard blockers) const noexcept;

  std::span<const Magic>
  GetMagics() const noexcept
  { return std::span(magics_); }

private:
  // The magic numbers for every square on the board.
  std::vector<Magic> magics_;
};

std::expected<MagicAttacks, Err>
ComputeBishopMagics();

std::expected<MagicAttacks, Err>
ComputeRookMagics();

std::expected<MagicAttacks, Err>
InitFromBishopMagics(std::span<const std::uint64_t> magics);

std::expected<MagicAttacks, Err>
InitFromRookMagics(std::span<const std::uint64_t> magics);

} // namespace blunder
