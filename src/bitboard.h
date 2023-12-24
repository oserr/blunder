#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "square_iter.h"

namespace blunder {

class BitBoard {
public:
  // Default ctor: initializes the BitBoard with all bits cleared.
  BitBoard()
    : bits(0ull) {}

  // Initializes the BitBoard with some bits.
  explicit
  BitBoard(std::uint64_t bits)
    : bits(bits) {}

  //----------------------------
  // Default copy control below.
  //----------------------------

  BitBoard(const BitBoard& other) noexcept = default;

  BitBoard(BitBoard&& other) noexcept = default;

  BitBoard&
  operator=(const BitBoard& other) noexcept = default;

  BitBoard&
  operator=(BitBoard&& other) noexcept = default;

  // Clears all the bits.
  BitBoard&
  clear() noexcept
  {
    bits = 0ull;
    return *this;
  }

  // Returns true if bit at index |index| is set.
  bool
  has_bit(unsigned index) noexcept
  { return bits & (1ull << index); }

  // Clears the bit set at index |index|. Doesn't have any effect if the bit is
  // not set.
  BitBoard&
  clear_bit(unsigned index) noexcept
  {
    bits &= ~(1ull << index);
    return *this;
  }

  // Sets the bit at the given index.
  BitBoard&
  set_bit(unsigned index) noexcept
  {
    bits |= 1ull << index;
    return *this;
  }

  BitBoard&
  update_bit(unsigned from, unsigned to)
  {
    // TODO: use assert(has_bit(from)) once I fix the bug.
    if (!has_bit(from))
      throw std::runtime_error("update_bit: from bit is not set.");

    // TODO: use assert(!has_bit(to)) once I fix the bug.
    if (has_bit(to))
      throw std::runtime_error("update_bit: to bit already set.");

    return clear_bit(from).set_bit(to);
  }

  // Sets the bits with other_bits.
  BitBoard&
  set_bits(std::uint64_t other_bits) noexcept
  {
    bits = other_bits;
    return *this;
  }

  // Returns the number of bits set.
  unsigned
  count() const noexcept
  { return std::popcount(bits); }

  // Returns true if the BitBoard only has one bit set, false otherwise.
  bool
  has_single_bit() const noexcept
  { return std::has_single_bit(bits); }

  // Returns the underlying bits.
  std::uint64_t
  raw() const noexcept
  { return bits; }

  // Returns the index of the first bit set starting from LSB. If no bits are
  // set, this will return 64.
  unsigned
  first_bit() const noexcept
  { return std::countr_zero(bits); }

  BitBoard& clear_first() noexcept
  {
    bits &= bits - 1;
    return *this;
  }

  // Like first_bit, but also clears the bit.
  unsigned
  first_bit_and_clear() noexcept
  {
    auto index = first_bit();
    clear_first();
    return index;
  }

  // Like first_bit_and_clear, but also returns a BitBoard with the bit set at
  // the index of the first bit set.
  std::pair<unsigned, BitBoard>
  index_bb_and_clear() noexcept
  {
    auto index = first_bit_and_clear();
    return {index, from_index(index)};
  }

  // Returns true if any bit is set, false otherwise.
  operator bool() const noexcept
  { return bits > 0; }

  // Returns true if bit at index is set.
  bool
  is_set(unsigned index) const noexcept
  { return static_cast<bool>(bits & (1ull << index)); }

  // Mutates the BitBoard by shifting left.
  BitBoard&
  sl(unsigned shift) noexcept
  {
    bits <<= shift;
    return *this;
  }

  // Returns a copy of the BitBoard shifted left.
  BitBoard
  shift_left(unsigned shift) const noexcept
  { return BitBoard(bits << shift); }

  // Rotates the bits 180 degrees, e.g. for flipping the colors.
  // Square a1 is mapped to h8, and a8 is mapped to h1. See
  // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#By_180_degrees_-_Bit-Reversal
  BitBoard&
  flip_this() noexcept;

  // Same as sl, but operator overload.
  BitBoard&
  operator<<=(unsigned shift) noexcept
  { return sl(shift); }

  // Mutates the BitBoard by shifting right.
  BitBoard&
  sr(unsigned shift) noexcept
  {
    bits >>= shift;
    return *this;
  }

  // Returns a copy of the BitBoard shifted right.
  BitBoard
  shift_right(unsigned shift) noexcept
  { return BitBoard(bits >> shift); }

  // Same as sr, but operator overload.
  BitBoard&
  operator>>=(unsigned shift) noexcept
  { return sr(shift); }

  // Operator overload for bit_or with assignment.
  BitBoard&
  operator|=(BitBoard bb) noexcept
  {
    bits |= bb.bits;
    return *this;
  }

  // Operator overload for bit_and with assignment.
  template<typename T>
  requires std::integral<T>
  BitBoard&
  operator&=(T other_bits) noexcept
  {
    bits &= static_cast<std::uint64_t>(other_bits);
    return *this;
  }

  // Operator overload for bit_and with assignment.
  BitBoard&
  operator&=(BitBoard bb) noexcept
  {
    bits &= bb.bits;
    return *this;
  }

  // Returns the complement of bits set.
  BitBoard
  bit_not() const noexcept
  { return BitBoard(~bits); }

  // Returns the insersection of bits with other.
  BitBoard
  bit_and(BitBoard other) const noexcept
  { return BitBoard(bits & other.bits); }

  // Returns the union of bits with other.
  BitBoard
  bit_or(BitBoard other) const noexcept
  { return BitBoard(bits | other.bits); }

  // String with 0s and 1s representing bit pattern.
  std::string
  str() const
  { return std::bitset<64>(bits).to_string(); }

  // Prints a bitboard in mailbox style, e.g.,
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | 1 |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | 1 |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | 1 |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | 1 |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | 1 |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | 1 |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | 1 |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | 1 |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  std::string
  fancy_str() const;

  // Returns true if bb is equal, i.e. they have the same set of bits set.
  bool
  eq(BitBoard bb) const noexcept
  { return bits == bb.bits; }

  // Creates a BitBoard with one bit set at the given index.
  static BitBoard
  from_index(unsigned index) noexcept
  { return BitBoard().set_bit(index); }

  SquareIter
  square_iter() const noexcept
  { return SquareIter(raw()); }

  BitBoard&
  swap(BitBoard& other) noexcept
  {
    std::swap(bits, other.bits);
    return *this;
  }

private:
  std::uint64_t bits;
};

inline auto
operator<=>(BitBoard left, BitBoard right) noexcept
{ return left.raw() <=> right.raw(); }

inline bool
operator==(BitBoard left, BitBoard right) noexcept
{ return left.eq(right); }

inline BitBoard
operator<<(BitBoard bb, unsigned shift) noexcept
{ return bb.shift_left(shift); }

inline BitBoard
operator>>(BitBoard bb, unsigned shift) noexcept
{ return bb.shift_right(shift); }

inline BitBoard
operator|(BitBoard left, BitBoard right) noexcept
{ return left.bit_or(right); }

template<typename T>
requires std::integral<T>
BitBoard
operator|(BitBoard bb, T mask) noexcept
{ return bb.bit_or(BitBoard(mask)); }

template<typename T>
requires std::integral<T>
BitBoard
operator|(T mask, BitBoard bb) noexcept
{ return bb | mask; }

inline BitBoard
operator&(BitBoard left, BitBoard right) noexcept
{ return left.bit_and(right); }

template<typename T>
requires std::integral<T>
BitBoard
operator&(BitBoard bb, T mask) noexcept
{ return bb.bit_and(BitBoard(mask)); }

template<typename T>
requires std::integral<T>
BitBoard
operator&(T mask, BitBoard bb) noexcept
{ return bb & mask; }

inline BitBoard
operator~(BitBoard bb) noexcept
{ return bb.bit_not(); }

inline std::ostream&
operator<<(std::ostream& os, BitBoard bb)
{ return os << bb.str(); }

template<typename T>
requires std::integral<T>
BitBoard
operator<<(BitBoard bb, T shift)
{ return bb.shift_left(shift); }

template<typename T>
requires std::integral<T>
BitBoard
operator>>(BitBoard bb, T shift)
{ return bb.shift_right(shift); }


// Initial white piece placement.
inline const BitBoard kWhiteKing(1ull << 4);
inline const BitBoard kWhiteQueen(1ull << 3);
inline const BitBoard kWhiteRooks((1ull << 7 ) | 1);
inline const BitBoard kWhiteKnights((1ull << 6 ) | (1 << 1));
inline const BitBoard kWhiteBishops((1ull << 5 ) | (1 << 2));
inline const BitBoard kWhitePawns(0xff00);
inline const BitBoard kWhitePieces = kWhiteKing |
                                kWhiteQueen |
                                kWhiteRooks |
                                kWhiteKnights |
                                kWhiteBishops |
                                kWhitePawns;

// Initial black piece placement.
inline const BitBoard kBlackKing = kWhiteKing << 56;
inline const BitBoard kBlackQueen = kWhiteQueen << 56;
inline const BitBoard kBlackRooks = kWhiteRooks << 56;
inline const BitBoard kBlackKnights = kWhiteKnights << 56;
inline const BitBoard kBlackBishops = kWhiteBishops << 56;
inline const BitBoard kBlackPawns = kWhitePawns << 40;
inline const BitBoard kBlackPieces = kBlackKing |
                                kBlackQueen |
                                kBlackRooks |
                                kBlackKnights |
                                kBlackBishops |
                                kBlackPawns;

// Files/colums.
inline const BitBoard kFileA(0x0101010101010101);
inline const BitBoard kFileB = kFileA << 1;
inline const BitBoard kFileC = kFileB << 1;
inline const BitBoard kFileD = kFileC << 1;
inline const BitBoard kFileE = kFileD << 1;
inline const BitBoard kFileF = kFileE << 1;
inline const BitBoard kFileG = kFileF << 1;
inline const BitBoard kFileH = kFileG << 1;

// Ranks/rows.
inline const BitBoard kRank1(0xff);
inline const BitBoard kRank2 = kRank1 << 8;
inline const BitBoard kRank3 = kRank2 << 8;
inline const BitBoard kRank4 = kRank3 << 8;
inline const BitBoard kRank5 = kRank4 << 8;
inline const BitBoard kRank6 = kRank5 << 8;
inline const BitBoard kRank7 = kRank6 << 8;
inline const BitBoard kRank8 = kRank7 << 8;

// All diagonals going up the board from A1 to A7.
inline const BitBoard kDiagA1H8(0x8040201008040201);
inline const BitBoard kDiagA2G8 = kDiagA1H8 << 8;
inline const BitBoard kDiagA3F8 = kDiagA2G8 << 8;
inline const BitBoard kDiagA4E8 = kDiagA3F8 << 8;
inline const BitBoard kDiagA5D8 = kDiagA4E8 << 8;
inline const BitBoard kDiagA6C8 = kDiagA5D8 << 8;
inline const BitBoard kDiagA7B8 = kDiagA6C8 << 8;

// All diagonals going up the board from B1 to G1.
inline const BitBoard kDiagB1H7 = kDiagA1H8 >> 8;
inline const BitBoard kDiagC1H6 = kDiagB1H7 >> 8;
inline const BitBoard kDiagD1H5 = kDiagC1H6 >> 8;
inline const BitBoard kDiagE1H4 = kDiagD1H5 >> 8;
inline const BitBoard kDiagF1H3 = kDiagE1H4 >> 8;
inline const BitBoard kDiagG1H2 = kDiagF1H3 >> 8;

// All diagonals going down the board from A8 to A2.
inline const BitBoard kDiagA8H1(0x0102040810204080);
inline const BitBoard kDiagA7G1 = kDiagA8H1 >> 8;
inline const BitBoard kDiagA6F1 = kDiagA7G1 >> 8;
inline const BitBoard kDiagA5E1 = kDiagA6F1 >> 8;
inline const BitBoard kDiagA4D1 = kDiagA5E1 >> 8;
inline const BitBoard kDiagA3C1 = kDiagA4D1 >> 8;
inline const BitBoard kDiagA2B1 = kDiagA3C1 >> 8;

// All diagonals going down the board from B8 to G8.
inline const BitBoard kDiagB8H2 = kDiagA8H1 << 8;
inline const BitBoard kDiagC8H3 = kDiagB8H2 << 8;
inline const BitBoard kDiagD8H4 = kDiagC8H3 << 8;
inline const BitBoard kDiagE8H5 = kDiagD8H4 << 8;
inline const BitBoard kDiagF8H6 = kDiagE8H5 << 8;
inline const BitBoard kDiagG8H7 = kDiagF8H6 << 8;

// Mask for the outer squares of the board. This is helpful for ignoring the
// outer squares when computing sliding attacks.
inline const BitBoard kOuterSquares = kFileA | kFileH | kRank1 | kRank8;

// Colored squres.
inline const BitBoard kLightSquares(0x55aa55aa55aa55aa);
inline const BitBoard kDarkSquares = ~kLightSquares;

inline const std::array<BitBoard, 64> kDiagMask= {
  // Rank 1
  kDiagA1H8,
  kDiagB1H7 | kDiagA2B1,
  kDiagC1H6 | kDiagA3C1,
  kDiagD1H5 | kDiagA4D1,
  kDiagE1H4 | kDiagA5E1,
  kDiagF1H3 | kDiagA6F1,
  kDiagG1H2 | kDiagA7G1,
  kDiagA8H1,
  // Rank 2
  kDiagA2G8 | kDiagA2B1,
  kDiagA1H8 | kDiagA3C1,
  kDiagB1H7 | kDiagA4D1,
  kDiagC1H6 | kDiagA5E1,
  kDiagD1H5 | kDiagA6F1,
  kDiagE1H4 | kDiagA7G1,
  kDiagF1H3 | kDiagA8H1,
  kDiagG1H2 | kDiagB8H2,
  // Rank 3
  kDiagA3F8 | kDiagA3C1,
  kDiagA2G8 | kDiagA4D1,
  kDiagA1H8 | kDiagA5E1,
  kDiagB1H7 | kDiagA6F1,
  kDiagC1H6 | kDiagA7G1,
  kDiagD1H5 | kDiagA8H1,
  kDiagE1H4 | kDiagB8H2,
  kDiagF1H3 | kDiagC8H3,
  // Rank 4
  kDiagA4E8 | kDiagA4D1,
  kDiagA3F8 | kDiagA5E1,
  kDiagA2G8 | kDiagA6F1,
  kDiagA1H8 | kDiagA7G1,
  kDiagB1H7 | kDiagA8H1,
  kDiagC1H6 | kDiagB8H2,
  kDiagD1H5 | kDiagC8H3,
  kDiagE1H4 | kDiagD8H4,
  // Rank 5
  kDiagA5D8 | kDiagA5E1,
  kDiagA4E8 | kDiagA6F1,
  kDiagA3F8 | kDiagA7G1,
  kDiagA2G8 | kDiagA8H1,
  kDiagA1H8 | kDiagB8H2,
  kDiagB1H7 | kDiagC8H3,
  kDiagC1H6 | kDiagD8H4,
  kDiagD1H5 | kDiagE8H5,
  // Rank 6
  kDiagA6C8 | kDiagA6F1,
  kDiagA5D8 | kDiagA7G1,
  kDiagA4E8 | kDiagA8H1,
  kDiagA3F8 | kDiagB8H2,
  kDiagA2G8 | kDiagC8H3,
  kDiagA1H8 | kDiagD8H4,
  kDiagB1H7 | kDiagE8H5,
  kDiagC1H6 | kDiagF8H6,
  // Rank 7
  kDiagA7B8 | kDiagA7G1,
  kDiagA6C8 | kDiagA8H1,
  kDiagA5D8 | kDiagB8H2,
  kDiagA4E8 | kDiagC8H3,
  kDiagA3F8 | kDiagD8H4,
  kDiagA2G8 | kDiagE8H5,
  kDiagA1H8 | kDiagF8H6,
  kDiagB1H7 | kDiagG8H7,
  // Rank 8
  kDiagA8H1,
  kDiagA7B8 | kDiagB8H2,
  kDiagA6C8 | kDiagC8H3,
  kDiagA5D8 | kDiagD8H4,
  kDiagA4E8 | kDiagE8H5,
  kDiagA3F8 | kDiagF8H6,
  kDiagA2G8 | kDiagG8H7,
  kDiagA1H8
};

inline const std::array<BitBoard, 64> kFileRankMask= {
  // Rank 1
  kFileA | kRank1,
  kFileB | kRank1,
  kFileC | kRank1,
  kFileD | kRank1,
  kFileE | kRank1,
  kFileF | kRank1,
  kFileG | kRank1,
  kFileH | kRank1,
  // Rank 2
  kFileA | kRank2,
  kFileB | kRank2,
  kFileC | kRank2,
  kFileD | kRank2,
  kFileE | kRank2,
  kFileF | kRank2,
  kFileG | kRank2,
  kFileH | kRank2,
  // Rank 3
  kFileA | kRank3,
  kFileB | kRank3,
  kFileC | kRank3,
  kFileD | kRank3,
  kFileE | kRank3,
  kFileF | kRank3,
  kFileG | kRank3,
  kFileH | kRank3,
  // Rank 4
  kFileA | kRank4,
  kFileB | kRank4,
  kFileC | kRank4,
  kFileD | kRank4,
  kFileE | kRank4,
  kFileF | kRank4,
  kFileG | kRank4,
  kFileH | kRank4,
  // Rank 5
  kFileA | kRank5,
  kFileB | kRank5,
  kFileC | kRank5,
  kFileD | kRank5,
  kFileE | kRank5,
  kFileF | kRank5,
  kFileG | kRank5,
  kFileH | kRank5,
  // Rank 6
  kFileA | kRank6,
  kFileB | kRank6,
  kFileC | kRank6,
  kFileD | kRank6,
  kFileE | kRank6,
  kFileF | kRank6,
  kFileG | kRank6,
  kFileH | kRank6,
  // Rank 7
  kFileA | kRank7,
  kFileB | kRank7,
  kFileC | kRank7,
  kFileD | kRank7,
  kFileE | kRank7,
  kFileF | kRank7,
  kFileG | kRank7,
  kFileH | kRank7,
  // Rank 8
  kFileA | kRank8,
  kFileB | kRank8,
  kFileC | kRank8,
  kFileD | kRank8,
  kFileE | kRank8,
  kFileF | kRank8,
  kFileG | kRank8,
  kFileH | kRank8,
};

inline const BitBoard kFileARank8FileH = kFileA | kRank8 | kFileH;
inline const BitBoard kRank1Rank8FileH = kRank1 | kRank8 | kFileH;
inline const BitBoard kRank1FileAFileH = kRank1 | kFileA | kFileH;
inline const BitBoard kFileARank1Rank8 = kFileA | kRank1 | kRank8;

// These masks ignore the outer squares. For example, for a1, we don't care
// about bits at a8 and h1.
inline const std::array<BitBoard, 64> kRookMask= {
  // Rank 1
  kFileRankMask[0] & ~(1ull | kFileH | kRank8),
  kFileRankMask[1] & ~((1ull << 1) | kFileARank8FileH),
  kFileRankMask[2] & ~((1ull << 2) | kFileARank8FileH),
  kFileRankMask[3] & ~((1ull << 3) | kFileARank8FileH),
  kFileRankMask[4] & ~((1ull << 4) | kFileARank8FileH),
  kFileRankMask[5] & ~((1ull << 5) | kFileARank8FileH),
  kFileRankMask[6] & ~((1ull << 6) | kFileARank8FileH),
  kFileRankMask[7] & ~((1ull << 7) | kFileA | kRank8),
  // Rank 2
  kFileRankMask[8] & ~((1ull << 8) | kRank1Rank8FileH),
  kFileRankMask[9] & ~((1ull << 9) | kOuterSquares),
  kFileRankMask[10] & ~((1ull << 10) | kOuterSquares),
  kFileRankMask[11] & ~((1ull << 11) | kOuterSquares),
  kFileRankMask[12] & ~((1ull << 12) | kOuterSquares),
  kFileRankMask[13] & ~((1ull << 13) | kOuterSquares),
  kFileRankMask[14] & ~((1ull << 14) | kOuterSquares),
  kFileRankMask[15] & ~((1ull << 15) | kFileARank1Rank8),
  // Rank 3
  kFileRankMask[16] & ~((1ull << 16) | kRank1Rank8FileH),
  kFileRankMask[17] & ~((1ull << 17) | kOuterSquares),
  kFileRankMask[18] & ~((1ull << 18) | kOuterSquares),
  kFileRankMask[19] & ~((1ull << 19) | kOuterSquares),
  kFileRankMask[20] & ~((1ull << 20) | kOuterSquares),
  kFileRankMask[21] & ~((1ull << 21) | kOuterSquares),
  kFileRankMask[22] & ~((1ull << 22) | kOuterSquares),
  kFileRankMask[23] & ~((1ull << 23) | kFileARank1Rank8),
  // Rank 4
  kFileRankMask[24] & ~((1ull << 24) | kRank1Rank8FileH),
  kFileRankMask[25] & ~((1ull << 25) | kOuterSquares),
  kFileRankMask[26] & ~((1ull << 26) | kOuterSquares),
  kFileRankMask[27] & ~((1ull << 27) | kOuterSquares),
  kFileRankMask[28] & ~((1ull << 28) | kOuterSquares),
  kFileRankMask[29] & ~((1ull << 29) | kOuterSquares),
  kFileRankMask[30] & ~((1ull << 30) | kOuterSquares),
  kFileRankMask[31] & ~((1ull << 31) | kFileARank1Rank8),
  // Rank 5
  kFileRankMask[32] & ~((1ull << 32) | kRank1Rank8FileH),
  kFileRankMask[33] & ~((1ull << 33) | kOuterSquares),
  kFileRankMask[34] & ~((1ull << 34) | kOuterSquares),
  kFileRankMask[35] & ~((1ull << 35) | kOuterSquares),
  kFileRankMask[36] & ~((1ull << 36) | kOuterSquares),
  kFileRankMask[37] & ~((1ull << 37) | kOuterSquares),
  kFileRankMask[38] & ~((1ull << 38) | kOuterSquares),
  kFileRankMask[39] & ~((1ull << 39) | kFileARank1Rank8),
  // Rank 6
  kFileRankMask[40] & ~((1ull << 40) | kRank1Rank8FileH),
  kFileRankMask[41] & ~((1ull << 41) | kOuterSquares),
  kFileRankMask[42] & ~((1ull << 42) | kOuterSquares),
  kFileRankMask[43] & ~((1ull << 43) | kOuterSquares),
  kFileRankMask[44] & ~((1ull << 44) | kOuterSquares),
  kFileRankMask[45] & ~((1ull << 45) | kOuterSquares),
  kFileRankMask[46] & ~((1ull << 46) | kOuterSquares),
  kFileRankMask[47] & ~((1ull << 47) | kFileARank1Rank8),
  // Rank 7
  kFileRankMask[48] & ~((1ull << 48) | kRank1Rank8FileH),
  kFileRankMask[49] & ~((1ull << 49) | kOuterSquares),
  kFileRankMask[50] & ~((1ull << 50) | kOuterSquares),
  kFileRankMask[51] & ~((1ull << 51) | kOuterSquares),
  kFileRankMask[52] & ~((1ull << 52) | kOuterSquares),
  kFileRankMask[53] & ~((1ull << 53) | kOuterSquares),
  kFileRankMask[54] & ~((1ull << 54) | kOuterSquares),
  kFileRankMask[55] & ~((1ull << 55) | kFileARank1Rank8),
  // Rank 8
  kFileRankMask[56] & ~((1ull << 56) | kRank1 | kFileH),
  kFileRankMask[57] & ~((1ull << 57) | kRank1FileAFileH),
  kFileRankMask[58] & ~((1ull << 58) | kRank1FileAFileH),
  kFileRankMask[59] & ~((1ull << 59) | kRank1FileAFileH),
  kFileRankMask[60] & ~((1ull << 60) | kRank1FileAFileH),
  kFileRankMask[61] & ~((1ull << 61) | kRank1FileAFileH),
  kFileRankMask[62] & ~((1ull << 62) | kRank1FileAFileH),
  kFileRankMask[63] & ~((1ull << 63) | kRank1 | kFileA)
};

inline BitBoard
get_rmask(std::uint32_t sq) noexcept
{
  assert(sq < 64);
  return kRookMask[sq];
}

inline BitBoard
get_bmask(std::uint32_t sq) noexcept
{
  assert(sq < 64);
  return kDiagMask[sq] & ~kOuterSquares & ~(1ull << sq);
}

BitBoard
get_rattacks(std::uint32_t sq, BitBoard blocking) noexcept;

BitBoard
get_battacks(std::uint32_t sq, BitBoard blocking) noexcept;

} // blunder
