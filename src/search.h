#pragma once

#include <vector>

#include "board.h"
#include "board_path.h"
#include "move.h"

namespace blunder {

// Packages together a move with a prior probability from the evaluator and the visit count from the MCTS.
struct MoveProb {
  Board board;
  float prior;
  unsigned visits;
};

inline bool
operator<(const MoveProb& left, const MoveProb& right) noexcept
{ return left.visits < right.visits; }

// TODO: Don't want to have two copies of the board in best and moves, so
// figure out a way to avoid the copy, e.g. have a pointer but make this into a
// class.
struct SearchResult {
  // The best move.
  MoveProb best;

  // Vector of moves with prior and posterior probabilities probabilities.
  std::vector<MoveProb> moves;

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
