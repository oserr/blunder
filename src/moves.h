#pragma once

#include "bitboard.h"

namespace blunder {

constexpr BitBoard
MoveKing(BitBoard king) noexcept;

constexpr BitBoard
MoveQueen(BitBoard queen) noexcept;

constexpr BitBoard
MoveRook(BitBoard rook) noexcept;

constexpr BitBoard
MoveBishop(BitBoard bishop) noexcept;

constexpr BitBoard
MoveKnight(BitBoard knight) noexcept;

// Returns the set of white pawns that can move one square forward.
constexpr BitBoard
WhiteMoveWhitePawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of white pawns that can move two squares forward.
constexpr BitBoard
MoveWhitePawnsDouble(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of black pieces attacked by white pawns on a left diagonal
// attack.
constexpr BitBoard
MoveWhitePawnsAttackLeft(BitBoard pawns, BitBoard all_black) noexcept;

// Returns the set of black pieces attached by black pawns on a right diagonal
// attack.
constexpr BitBoard
MoveWhitePawnsAttackRight(BitBoard pawns, BitBoard all_black) noexcept;

// Returns the set of black pawns that can move one square forward.
constexpr BitBoard
MoveBlackPawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of black pawns that can move one square forward.
constexpr BitBoard
MoveBlackPawnsSingle(BitBoard pawns, BitBoard empty) noexcept;

// Returns the set of white pieces attacked by black pawns on a left diagonal
// attack.
constexpr BitBoard
MoveBlackPawnsAttackLeft(BitBoard pawns, BitBoard all_white) noexcept;

// Returns the set of white pieces attached by black pawns on a right diagonal
// attack.
constexpr BitBoard
MoveBlackPawnsAttackRight(BitBoard pawns, BitBoard all_white) noexcept;

}
