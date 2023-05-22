#pragma once

#include "bitboard.h"

namespace blunder {

constexpr BitBoard
MoveKing(BitBoard king) noexcept;

// Returns the set of squares where a king can move, except for castling.
constexpr BitBoard
MoveKnight(BitBoard knight) noexcept;

// Returns the set of white pawns after moving one square forward.
constexpr BitBoard
WhiteMoveWhitePawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of white pawns afer moving two squares forward for pawns that
// can move two squares forward.
constexpr BitBoard
MoveWhitePawnsDouble(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of white pawns after attacking diagonal left.
constexpr BitBoard
MoveWhitePawnsAttackLeft(BitBoard pawns, BitBoard all_black) noexcept;

// Returns the set of white pawns after attacking diagonal right.
constexpr BitBoard
MoveWhitePawnsAttackRight(BitBoard pawns, BitBoard all_black) noexcept;

// Returns the set of black pawns after moving one square forward.
constexpr BitBoard
MoveBlackPawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of black pawns after moving two squares forward for pawns
// that can move two squares forward.
constexpr BitBoard
MoveBlackPawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of black pawns after attacking diagonal left.
constexpr BitBoard
MoveBlackPawnsAttackLeft(BitBoard pawns, BitBoard all_white) noexcept;

// Returns the set of black pawns after attacking diagonal right.
constexpr BitBoard
MoveBlackPawnsAttackRight(BitBoard pawns, BitBoard all_white) noexcept;

}
