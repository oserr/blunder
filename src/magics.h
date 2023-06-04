#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <span>
#include <utility>
#include <vector>

#include "bitboard.h"
#include "err.h"

namespace blunder {

struct Magic {
  std::vector<BitBoard> attacks;
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
