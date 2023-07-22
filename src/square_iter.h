#pragma once

#include <bit>
#include <cstdint>

namespace blunder {

// For iterating on the bits of a bitboard as if each bit was the index of the
// square on the board where the bit is set.
class SquareIter {
public:
  // Default ctor initializes raw bits to zero. This is the end iterator.
  SquareIter() noexcept
    : bits(0ull) {}

  // Initializes the iterator with a BitBoard's raw bits.
  SquareIter(std::uint64_t bits)
    : bits(bits) {}

  SquareIter
  begin() const noexcept
  { return *this; }

  SquareIter
  end() const noexcept
  { return SquareIter(); }

  unsigned
  operator*() const noexcept
  { return std::countr_zero(bits); }

  // Prefix ++.
  SquareIter&
  operator++() noexcept
  {
    bits &= bits - 1;
    return *this;
  }

  // Postfix ++.
  SquareIter
  operator++(int) noexcept
  {
    SquareIter si(bits);
    ++*this;
    return si;
  }

  bool
  operator==(const SquareIter& other) const noexcept
  { return bits == other.bits; }

  bool
  operator!=(const SquareIter& other) const noexcept
  { return bits != other.bits; }

private:
  std::uint64_t bits;
};

} // namespace blunder
