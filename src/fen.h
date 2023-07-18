#pragma once

#include <expected>
#include <ostream>
#include <string_view>

#include "board.h"

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

// Returns the Board representing the chess position in the |fen| string,
// or an error code if unable to parse the position.
std::expected<Board, FenErr>
read_fen(std::string_view fen) noexcept;

std::string_view
str_view(FenErr err) noexcept;

inline std::ostream&
operator<<(std::ostream& os, FenErr err)
{ return os << str_view(err); }

} // namespace blunder
