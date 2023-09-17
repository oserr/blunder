#pragma once

#include <vector>

#include "board.h"
#include "board_path.h"
#include "move.h"
#include "search_result.h"

namespace blunder {

// An interface for searching a chess game tree.
class Search {
public:
  virtual ~Search() = default;

  // Runs a game search.
  virtual SearchResult
  run(const EvalBoardPath& board) const = 0;
};

} // namespace blunder
