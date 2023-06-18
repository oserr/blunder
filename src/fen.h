#pragma once

#include <expected>
#include <ostream>
#include <string_view>

#include "board_state.h"

namespace blunder {

// An enum class to represent parsing errors for the different fields in FEN
// string.
enum class FenErr {
  NoPieces,
  NoColor,
  NoCastling,
  NoEnPassant,
  NoHalfMove,
  NoFullMove,
  InvalidColor,
  InvalidCastling,
  InvalidEnPassant,
  InvalidHalfMove,
  InvalidFullMove,
  MissingField,
  UnknownPiece,
  WhiteNotLogical,
  BlackNotLogical,
  Not64Squares,
  InvalidRow,
  MissingRows,
  ExtraRows,
  InvalidNum,
  Internal,
};

// Returns the BoardState representing the chess position in the |fen| string,
// or an error code if unable to parse the position.
std::expected<BoardState, FenErr>
ReadFen(std::string_view fen) noexcept;

std::string_view
StrView(FenErr err) noexcept;

inline std::ostream&
operator<<(std::ostream& os, FenErr err)
{
  os << StrView(err);
  return os;
}

} // namespace blunder