#pragma once

#include <string_view>

namespace blunder {

enum class GameWinner {
  White,
  Black,
  Draw,
  // Game did not finish, for example.
  None
};

std::string_view
str(GameWinner game_winner) noexcept;

} // namespace blunder
