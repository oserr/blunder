#pragma once

namespace blunder {

// Packages together a board move with a prior probability from the evaluator
// and the visit count from the MCTS.
struct BoardProb {
  Board board;
  float prior = 0.0;
  unsigned visits = 0;
};

inline bool
operator<(const BoardProb& left, const BoardProb& right) noexcept
{ return left.visits < right.visits; }

// TODO: Don't want to have two copies of the board in best and moves, so
// figure out a way to avoid the copy, e.g. have a pointer but make this into a
// class.
//
// TODO: Include other search statics, e.g.,
// - total number of nodes searched
// - total time searching
// - depth
struct SearchResult {
  // The best move.
  BoardProb best;

  // Vector of moves with prior and posterior probabilities probabilities.
  std::vector<BoardProb> moves;

  // The expected value of winning from this position for the current player.
  float value;
};

} // namespace blunder
