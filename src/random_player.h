#pragma once

#include <algorithm>
#include <span>
#include <string_view>
#include <utility>

#include "board.h"
#include "board_path.h"
#include "player.h"
#include "random_search.h"

namespace blunder {

// Implements a random player.
class RandomPlayer : public Player {
public:
  RandomPlayer(unsigned seed)
     : rand_search(seed) {}

  PlayResult
  make_move(const GameBoardPath& boards) override
  {
    auto board_path = EvalBoardPath::rev(boards);
    auto result = rand_search.run(board_path);
    return PlayResult::take_from(result);
  }

  std::string_view
  name() const noexcept
  { return "RandomPlayer"; }

private:
  RandomSearch rand_search;
};

} // namespace blunder
