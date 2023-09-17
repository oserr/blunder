#include "random_search.h"

#include <random>
#include <stdexcept>
#include <vector>

namespace blunder {

SearchResult
RandomSearch::run(const BoardPath& board_path) const
{
  auto root = board_path.root();
  if (not root)
    throw std::logic_error("BoardPath should have a root.");

  const auto& board = root->get();
  if (board.is_terminal())
    throw std::invalid_argument("Board is in a terminal state.");

  auto children = board.next();
  if (children.empty())
    throw std::length_error("children should be non-empty.");

  std::vector<BoardProb> moves;
  moves.reserve(children.size());
  for (unsigned i = 0; i < children.size(); ++i)
    moves.emplace_back().board = std::move(children[i]);

  std::uniform_int_distribution<unsigned> rand_gen(0, moves.size()-1);
  for (unsigned i = 0; i < 100; ++i) {
    auto index = rand_gen(rand_fn);
    ++moves[index].visits;
  }

  for (auto& mv : moves)
    mv.prior = static_cast<float>(mv.visits) / 100.0;

  auto iter = std::max_element(moves.begin(), moves.end());

  SearchResult result {
    .best = *iter,
    .moves = std::move(moves),
    .value = rand_value_fn(rand_fn)
  };

  return result;
}

} // namespace blunder
