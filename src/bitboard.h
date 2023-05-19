#pragma once

#include <array>
#include <cassert>
#include <cstdint>

namespace blunder {

using BitBoard = std::uint64_t;

// Initial white piece placement.
inline constexpr BitBoard kWhiteKing = 1 << 4;
inline constexpr BitBoard kWhiteQueen = 1 << 3;
inline constexpr BitBoard kWhiteRooks = (1 << 7 ) | 1;
inline constexpr BitBoard kWhiteKnights = (1 << 6 ) | (1 << 1);
inline constexpr BitBoard kWhiteBishops = (1 << 5 ) | (1 << 2);
inline constexpr BitBoard kWhitePawns = 0xff00;
inline constexpr BitBoard kWhitePieces = kWhiteKing |
                                kWhiteQueen |
                                kWhiteRooks |
                                kWhiteKnights |
                                kWhiteBishops |
                                kWhitePawns;

// Initial black piece placement.
inline constexpr BitBoard kBlackKing = kWhiteKing << 56;
inline constexpr BitBoard kBlackQueen = kWhiteQueen << 56;
inline constexpr BitBoard kBlackRooks = kWhiteRooks << 56;
inline constexpr BitBoard kBlackKnights = kWhiteKnights << 56;
inline constexpr BitBoard kBlackBishops = kWhiteBishops << 56;
inline constexpr BitBoard kBlackPawns = kWhitePawns << 40;
inline constexpr BitBoard kBlackPieces = kBlackKing |
                                kBlackQueen |
                                kBlackRooks |
                                kBlackKnights |
                                kBlackBishops |
                                kBlackPawns;

// Files/colums.
inline constexpr BitBoard kFileA = 0x0101010101010101;
inline constexpr BitBoard kFileB = kFileA << 1;
inline constexpr BitBoard kFileC = kFileB << 1;
inline constexpr BitBoard kFileD = kFileC << 1;
inline constexpr BitBoard kFileE = kFileD << 1;
inline constexpr BitBoard kFileF = kFileE << 1;
inline constexpr BitBoard kFileG = kFileF << 1;
inline constexpr BitBoard kFileH = kFileG << 1;

// Ranks/rows.
inline constexpr BitBoard kRank1 = 0xff;
inline constexpr BitBoard kRank2 = kRank1 << 8;
inline constexpr BitBoard kRank3 = kRank2 << 8;
inline constexpr BitBoard kRank4 = kRank3 << 8;
inline constexpr BitBoard kRank5 = kRank4 << 8;
inline constexpr BitBoard kRank6 = kRank5 << 8;
inline constexpr BitBoard kRank7 = kRank6 << 8;
inline constexpr BitBoard kRank8 = kRank7 << 8;

// All diagonals going up the board from A1 to A7.
inline constexpr BitBoard kDiagA1H8 = 0x8040201008040201;
inline constexpr BitBoard kDiagA2G8 = kDiagA1H8 << 8;
inline constexpr BitBoard kDiagA3F8 = kDiagA2G8 << 8;
inline constexpr BitBoard kDiagA4E8 = kDiagA3F8 << 8;
inline constexpr BitBoard kDiagA5D8 = kDiagA4E8 << 8;
inline constexpr BitBoard kDiagA6C8 = kDiagA5D8 << 8;
inline constexpr BitBoard kDiagA7B8 = kDiagA6C8 << 8;

// All diagonals going up the board from B1 to G1.
inline constexpr BitBoard kDiagB1H7 = kDiagA1H8 >> 8;
inline constexpr BitBoard kDiagC1H6 = kDiagB1H7 >> 8;
inline constexpr BitBoard kDiagD1H5 = kDiagC1H6 >> 8;
inline constexpr BitBoard kDiagE1H4 = kDiagD1H5 >> 8;
inline constexpr BitBoard kDiagF1H3 = kDiagE1H4 >> 8;
inline constexpr BitBoard kDiagG1H2 = kDiagF1H3 >> 8;

// All diagonals going down the board from A8 to A2.
inline constexpr BitBoard kDiagA8H1 = 0x0102040810204080;
inline constexpr BitBoard kDiagA7G1 = kDiagA8H1 >> 8;
inline constexpr BitBoard kDiagA6F1 = kDiagA7G1 >> 8;
inline constexpr BitBoard kDiagA5E1 = kDiagA6F1 >> 8;
inline constexpr BitBoard kDiagA4D1 = kDiagA5E1 >> 8;
inline constexpr BitBoard kDiagA3C1 = kDiagA4D1 >> 8;
inline constexpr BitBoard kDiagA2B1 = kDiagA3C1 >> 8;

// All diagonals going down the board from B8 to G8.
inline constexpr BitBoard kDiagB8H2 = kDiagA8H1 << 8;
inline constexpr BitBoard kDiagC8H3 = kDiagB8H2 << 8;
inline constexpr BitBoard kDiagD8H4 = kDiagC8H3 << 8;
inline constexpr BitBoard kDiagE8H5 = kDiagD8H4 << 8;
inline constexpr BitBoard kDiagF8H6 = kDiagE8H5 << 8;
inline constexpr BitBoard kDiagG8H7 = kDiagF8H6 << 8;

// Mask for the outer squares of the board. This is helpful for ignoring the
// outer squares when computing sliding attacks.
inline constexpr BitBoard kOuterSquares = kFileA | kFileH | kRank1 | kRank8;

// Colored squres.
inline constexpr BitBoard kLightSquares = 0x55aa55aa55aa55aa;
inline constexpr BitBoard kDarkSquares = ~kLightSquares;

inline constexpr BitBoard k64Bits = 64;

inline constexpr std::array<BitBoard, k64Bits> kDiagMask= {
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

inline constexpr std::array<BitBoard, k64Bits> kFileRankMask= {
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

inline BitBoard
GetRookMask(std::uint32_t sq) noexcept
{
  assert(sq < 64);
  return kFileRankMask[sq] & ~kOuterSquares & ~(1ull << sq);
}

inline BitBoard
GetBishopMask(std::uint32_t sq) noexcept
{
  assert(sq < 64);
  return kDiagMask[sq] & ~kOuterSquares & ~(1ull << sq);
}

BitBoard
GetRookAttacks(std::uint32_t sq, BitBoard blocking) noexcept;

BitBoard
GetBishopAttacks(std::uint32_t sq, BitBoard blocking) noexcept;

} // blunder
