#pragma once

#include <cassert>
#include <vector>

#include "board.h"
#include "move.h"

namespace blunder {

// Packages together a board move with a prior probability from the evaluator
// and the visit count from the MCTS.
struct BoardProb {
  Board board;
  float prior = 0.0;
  unsigned visits = 0;
};

struct MoveProb {
  Move mv;
  float prior = 0.0;
  unsigned visits = 0;

  // Creates a MoveProb from a BoardProb. It's an error pass in a MoveProb with
  // a board without a valid last_move.
  static MoveProb
  from(const BoardProb& board_prob) noexcept
  {
    auto last_move = board_prob.board.last_move();
    assert(last_move);

    return MoveProb{
      .mv=*last_move,
      .prior=board_prob.prior,
      .visits=board_prob.visits
    };
  }
};

inline bool
operator<(const BoardProb& left, const BoardProb& right) noexcept
{ return left.visits < right.visits; }

inline bool
operator<(const MoveProb& left, const MoveProb& right) noexcept
{ return left.visits < right.visits; }

struct SearchResult {
  // The best move.
  BoardProb best;

  // Vector of moves with prior and posterior probabilities.
  std::vector<MoveProb> moves;

  // The expected value of winning from this position for the current player.
  float value = 0;

  // Total nodes searched, i.e. nodes that are expanded.
  unsigned num_expanded = 0;

  // Total nodes visited, including repeat visits.
  unsigned num_visited = 0;

  // The maximum depth of a branch explored during search.
  unsigned depth = 0;

  // Average number of milliseconds per evaluation of node.
  float millis_per_eval = 0;

  // Total number of milliseconds spent on evaluation.
  float millis_eval = 0;

  // Total number of milliseconds during search time.
  float millis_search_time = 0;
};

} // namespace blunder
