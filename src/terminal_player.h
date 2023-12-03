#pragma once

#include <string>

#include "board_path.h"
#include "player.h"
#include "search_result.h"

namespace blunder {

// Implements the player interface for a human playing on the terminal on the
// same computer.
class TerminalPlayer : public Player {
public:
  TerminalPlayer(std::string_view name)
    : player_name(name) {}

  SearchResult
  make_move(const GameBoardPath& boards) override;

  std::string_view
  name() const noexcept override
  { return player_name; }

private:
  std::string player_name;
};

} // namespace blunder
