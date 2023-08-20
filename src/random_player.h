#pragma once

#include <span>
#include <string_view>
#include <utility>

#include "board.h"
#include "board_path.h"
#include "random_search.h"

namespace blunder {

// Implements a random player.
class RandomPlayer : public Player {
public:
  RandomPlayer(unsigned seed)
     : rand_search(seed);

  Board
  make_move(std::span<const Board> boards) override
  {
    auto board_path = BoardPath::rev(boards);
    auto result = rand_search.run(board_path);
    return std::move(result.best.board);
  }

  std::string_view
  name() const noexcept
  { return "RandomPlayer"; }

private:
  RandomSearch rand_search;
};

} // namespace blunder
