#pragma once

#include <cstdint>

namespace blunder {

// TODO: we can add more specific enums for drawing later on, e.g. 3-move
// repetition.
enum class GameState : std::uint8_t {
  Playing,
  Check,
  Mate,
  Draw,
};

} // namespace blunder
