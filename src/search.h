#pragma once

#include <vector>

#include "board.h"
#include "board_path.h"
#include "move.h"

namespace blunder {

// Packages together a move with a prior and posterior probabilities.
struct MoveProb {
  Move mv;
  float prior;
  float prob;
};

struct SearchResult {
  // Vector of moves with prior and posterior probabilities probabilities.
  std::vector<MoveProb> moves;

  // The best move.
  Move best_move;

  // The expected value of winning from this position for the current player.
  float value;
};

// An interface for searching a chess game tree.
class Search {
public:
  virtual ~Search() = default;

  // Runs a game search.
  virtual SearchResult
  run(const BoardPath& board) const = 0;
};

} // namespace blunder
