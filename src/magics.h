#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "bitboard.h"

namespace blunder {

// Represents the magic attacks for a given square. For example, for bishop
// attacks, we'll have 64 of these.
struct Magic {
  // A table of attacks for a given permutation of blockers in the relevant
  // squares.
  std::vector<BitBoard> attacks;

  // A mask for the squares that may be attacked by a sliding piece from the
  // square associated with this Magic.
  BitBoard mask;

  // The magic number used to hash pattern of blockers against an attack
  // bitboard in attacks.
  std::uint64_t magic = 0;

  // The number of bits in mask.
  std::uint8_t nbits = 0;
};

// Simple interface for magic attacks, i.e. sliding piece attacks for rook,
// bishop, and queen moves.
class Magics {
public:
  // Returns a BitBoard with bits set to all squares attacked by a sliding
  // piece. The attacks are from |square|, and blockers is a bitboard with all
  // pieces, including white and black pieces.
  virtual BitBoard
  get_attacks(std::uint8_t square, BitBoard blockers) const noexcept = 0;

  // Returns the underlyinig magics used to compute the attacks.
  virtual std::span<const Magic>
  get_magics() const noexcept = 0;
};

} // namespace blunder
