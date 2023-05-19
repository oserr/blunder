#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>
#include <vector>

#include "bitboard.h"
#include "error.h"

namespace blunder {

struct Magic {
  std::vector<BitBoard> attacks;
  std::uint64_t magic = 0;
  std::uint8_t nbits = 0;
};

class MagicAttacks {
public:
  static std::expected<MagicAttacks, Error>
  ComputeBishopMagics();

  static MagicAttacks
  ComputeRookMagics();

  static MagicAttacks
  InitFromBishopMagics(std::span<std::uint64_t> magics);

  static MagicAttacks
  InitFromRookMagics(std::span<std::uint64_t> magics);

  BitBoard
  GetAttacks(std::uint8_t square, BitBoard blockers) const noexcept;

  std::span<const Magic>
  GetMagics() const noexcept;

private:
  explicit MagicAttacks(
    std::vector<Magic> magics)
    : magics_(std::move(magics)) {}

  // The magic numbers for every square on the board.
  std::vector<Magic> magics_;
};

} // namespace blunder
