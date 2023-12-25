#include "game_winner.h"

#include <string_view>

namespace blunder {

std::string_view
str(GameWinner game_winner) noexcept
{
  switch (game_winner) {
    case GameWinner::White:
      return "White";
    case GameWinner::Black:
      return "Black";
    case GameWinner::Draw:
      return "Draw";
    default:
      return "None";
  }
}

} // namespace blunder
