#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include "bitboard.h"
#include "err.h"
#include "magics.h"

#include "par.h"

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

class MagicAttacks : public Magics {
public:
  explicit MagicAttacks(
    std::vector<Magic> magics)
    : magics_(std::move(magics)) {}

  MagicAttacks(const MagicAttacks& magic_attacks) noexcept = default;
  MagicAttacks(MagicAttacks&& magic_attacks) noexcept = default;

  BitBoard
  get_attacks(std::uint8_t square, BitBoard blockers) const noexcept override;

  std::span<const Magic>
  get_magics() const noexcept override
  { return std::span(magics_); }

private:
  // The magic numbers for every square on the board.
  std::vector<Magic> magics_;
};

class MagicComputer {
public:
  virtual ~MagicComputer() = default;

  // Computes bishop magics from scratch.
  virtual MagicAttacks
  compute_bmagics() const = 0;

  // Computes rook magics from scratch.
  virtual MagicAttacks
  compute_rmagics() const = 0;

  // Initializes bishop magics from precomputed |magics|.
  virtual MagicAttacks
  from_bmagics(std::span<const std::uint64_t> magics) const = 0;

  // Initializes rook magics from precomputed |magics|.
  virtual MagicAttacks
  from_rmagics(std::span<const std::uint64_t> magics) const = 0;
};

class SimpleMagicComputer : public MagicComputer {
public:
  // Computes bishop magics from scratch.
  MagicAttacks
  compute_bmagics() const override;

  // Computes rook magics from scratch.
  MagicAttacks
  compute_rmagics() const override;

  // Initializes bishop magics from precomputed |magics|.
  MagicAttacks
  from_bmagics(std::span<const std::uint64_t> magics) const override;

  // Initializes rook magics from precomputed |magics|.
  MagicAttacks
  from_rmagics(std::span<const std::uint64_t> magics) const override;
};

class ParMagicComputer : public MagicComputer {
public:
  ParMagicComputer(std::shared_ptr<par::WorkQ> workq)
    : workq(std::move(workq))
  {
    if (not this->workq)
      throw std::invalid_argument("workq is null.");
  }

  // Computes bishop magics from scratch.
  MagicAttacks
  compute_bmagics() const override;

  // Computes rook magics from scratch.
  MagicAttacks
  compute_rmagics() const override;

  // Initializes bishop magics from precomputed |magics|.
  MagicAttacks
  from_bmagics(std::span<const std::uint64_t> magics) const override;

  // Initializes rook magics from precomputed |magics|.
  MagicAttacks
  from_rmagics(std::span<const std::uint64_t> magics) const override;

private:
  std::shared_ptr<par::WorkQ> workq;
};

// Computes bishop magics from scratch.
MagicAttacks
compute_bmagics();

// Computes rook magics from scratch.
MagicAttacks
compute_rmagics();

// Initializes bishop magics from precomputed |magics|.
MagicAttacks
from_bmagics(std::span<const std::uint64_t> magics);

// Initializes rook magics from precomputed |magics|.
MagicAttacks
from_rmagics(std::span<const std::uint64_t> magics);

} // namespace blunder
