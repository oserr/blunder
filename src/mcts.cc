#include "mcts.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace blunder {
namespace {

// TODO: figure out the values for these constants.
constexpr float EXPLORE_BASE = 19652;
constexpr float EXPLORE_INIT = 1.25;

constexpr float DIR_ALPHA = 0.3;
constexpr float DIR_EXPLORE_FRAC = 0.25;

// TODO: make this into a proper class to protect invariants when I settle down
// on its API.
struct Node {
  explicit
  Node(const Board& board) noexcept
    : board(board) {}

  Board board;
  std::vector<Node> children;
  MoveProb prob;
  Node* parent = nullptr;
  unsigned visit_count = 1;
  unsigned total_value = 0;
  unsigned mean_value = 0;
  bool is_leaf = true;

  Node*
  choose_next() noexcept
  { return is_leaf ? nullptr : choose_action(); }

  // Chooses the next child based on max{Q(s, a) + U(s, a)}.
  Node*
  choose_action() noexcept;

  // TODO: implement expand.
  void
  expand(const Prediction& pred) noexcept
  { (void)pred; }

  // TODO: implement expand. Here we take the priors and value from a previously
  // expanded node.
  void
  expand(const Node& other) noexcept
  { (void)other; }

  // TODO: implement is_terminal.
  bool
  is_terminal() const noexcept
  { return false; }

  // TODO: implement terminate.
  void
  terminate() noexcept
  { return; }

  // Computes the upper confidence bound. Asserts that parent is not null.
  float
  uct() const noexcept;

  // Returns the exploration term to compute UCT.
  float
  explore_rate() const noexcept;

  // Returns {Q(s, a) + U(s, a)}, where Q(s, a) is the mean action value and
  // U(s, a) is the upper confidence bound.
  float
  mean_uct() const noexcept
  { return mean_value + uct(); }

  // Define less than operator to make it more ergonomic to select the best node.
  bool
  operator<(const Node& right) const noexcept
  { return mean_uct() < right.mean_uct(); }
};

Node*
Node::choose_action() noexcept
{
  assert(not children.empty());
  auto iter = std::max_element(children.begin(), children.end());
  return iter == children.end() ? nullptr : &*iter;
}

float
Node::explore_rate() const noexcept
{
  assert(parent != nullptr);
  float num = 1 + parent->visit_count + EXPLORE_BASE;
  return std::log(num / EXPLORE_BASE) + EXPLORE_INIT;
}

float
Node::uct() const noexcept
{
  assert(parent != nullptr);
  float term1 = explore_rate() * prob.prior;
  float term2 = std::sqrt(parent->visit_count) / (1 + visit_count);
  return term1 * term2;
}

class GameTree {
public:
  explicit
  GameTree(const Board& board, const BoardPath& bp)
    : root(board),
      board_path(&bp)
  { assert(bp.size() > 0); }

  // Guaranteed to be non-null.
  Node*
  get_root() noexcept
  { return &root; }

  // TODO: implement get_path.
  BoardPath
  get_path(Node* node) const noexcept
  {
    (void)node;
    return BoardPath();
  }

  // TODO: implement update_stats.
  void
  update_stats(Node* node)
  { (void)node; }

  // TODO: implement find_expaned. It will find a node with the board state
  // that has already been expanded.
  const Node*
  find_expanded(Node* node)
  { return node; }

private:
  Node root;
  // Guaranteed to non-null.
  const BoardPath* board_path;
};

} // namespace

SearchResult
Mcts::run(const BoardPath& board_path) const
{
  auto board = board_path.root();
  if (not board)
    throw std::invalid_argument("BoardPath should have a root.");

  // TODO: when the root node is expanded, we need to add Dir noise to the prior
  // probabilities.
  GameTree game_tree(board->get(), board_path);

  for (unsigned i = 0; i < simuls; ++i) {
    auto* node = game_tree.get_root();

    while (not node->is_leaf and not node->is_terminal()) {
      node = node->choose_next();
      assert(node);
    }

    // We reached a terminal state so there is no need to call evaluator.
    if (node->is_terminal()) {
      node->terminate();
      game_tree.update_stats(node);
      continue;
    }

    auto other_node = game_tree.find_expanded(node);
    if (other_node) {
      // Copy priors and value to avoid calling evaluator.
      node->expand(*other_node);
      game_tree.update_stats(node);
      continue;
    }

    // Reached a leaf node.
    auto bp = game_tree.get_path(node);

    // Evaluate leaf node.
    auto pred = evaluator->predict(bp);

    // Expand leaf node.
    node->expand(pred);

    // Update stats.
    game_tree.update_stats(node);
  }

  // High level algo:
  // - Make root node for board.
  // - for simulation in simulations
  //   - traverse tree
  //   - if reach a leaf node expand and score
  return SearchResult();
}

} // namespace blunder
