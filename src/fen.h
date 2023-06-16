#pragma once

#include <expected>
#include <string_view>

#include "board_state.h"

namespace blunder {

// An enum class to represent parsing errors for the different fields in FEN
// string.
enum class FenErr {
  Pieces,
  Color,
  Castling,
  EnPassant,
  HalfMove,
  FullMove,
  MissingField,
  Unknown
};

// Returns the BoardState representing the chess position in the |fen| string,
// or an error code if unable to parse the position.
std::expected<BoardState, FenErr>
ReadFen(std::string_view fen) noexcept;

} // namespace blunder
