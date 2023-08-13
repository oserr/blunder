#include "mcts.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>

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
  // Initializes the Node with the Board.
  explicit
  Node(const Board& board)
    : board(board) {}

  // Initializes the node with the board and the prior.
  Node(const Board& board, float prior)
    : board(board), prior(prior) {}

  // Initializes the node with the board and the prior.
  Node(Board&& board, float prior)
    : board(std::move(board)), prior(prior) {}

  // Initializes the node with the board, parent, and prior.
  Node(Board&& board, Node& parent, float prior)
    : board(std::move(board)),
      parent(&parent),
      prior(prior) {}

  // Initializes the node with the board, parent, and prior.
  Node(const Board& board, Node& parent, float prior)
    : board(board),
      parent(&parent),
      prior(prior) {}

  Board board;
  std::vector<Node> children;
  Node* parent = nullptr;
  float prior;
  unsigned visits = 1;
  float value = 0.0;
  // The initial value from the network or from a termina state.
  float init_value = 0.0;
  bool is_leaf = true;

  Node*
  choose_next() noexcept
  { return is_leaf ? nullptr : choose_action(); }

  // Chooses the next child based on max{Q(s, a) + U(s, a)}.
  Node*
  choose_action() noexcept;

  // Expands the node with the move probabilities and the value from the
  // prediction.
  Node&
  expand(Prediction pred);

  // Expands the node using priors and value from node with same board state.
  Node&
  expand(const Node& other);

  // Returns true if the board reached a terminal state.
  bool
  is_terminal() const noexcept
  { return board.is_terminal(); }

  void
  terminate() noexcept;

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
  { return value / visits + uct(); }

  // Define less than operator to make it more ergonomic to select the best
  // node.
  bool
  operator<(const Node& right) const noexcept
  { return mean_uct() < right.mean_uct(); }

  // Propagates search statistics back up the search tree.
  void
  update_stats() noexcept;
};

Node&
Node::expand(Prediction pred)
{
  assert(not is_leaf and not is_terminal());

  if (pred.move_probs.empty())
    throw std::logic_error("No moves found, but expecting some.");

  is_leaf = false;
  value = pred.value;
  init_value = value;
  children.reserve(pred.move_probs.size());

  for (auto& [child_board, child_prior] : pred.move_probs)
    children.emplace_back(std::move(child_board), *this, child_prior);

  return *this;
}

Node&
Node::expand(const Node& other)
{
  assert(not other.children.empty());

  is_leaf = false;
  init_value = other.init_value;
  value = init_value;
  children.reserve(other.children.size());

  for (const auto& node : other.children)
    children.emplace_back(node.board, *this, node.prior);

  return *this;
}

void
Node::update_stats() noexcept
{
  auto node = parent;
  auto val = value;

  while (node) {
    val *= -1;
    ++node->visits;
    node->value += val;
    node = node->parent;
  }
}

void
Node::terminate() noexcept
{
  // Don't do anything if the board is not actually in a terminal state.
  if (not board.is_terminal())
    return;

  // The value of 1 here is for the move leading up to the check.
  value = board.is_mate() ? 1 : 0;
  init_value = value;
}

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
  float num = 1 + parent->visits + EXPLORE_BASE;
  return std::log(num / EXPLORE_BASE) + EXPLORE_INIT;
}

float
Node::uct() const noexcept
{
  assert(parent != nullptr);
  float term1 = explore_rate() * prior;
  float term2 = std::sqrt(parent->visits) / (1 + visits);
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
      node->update_stats();
      continue;
    }

    auto other_node = game_tree.find_expanded(node);
    if (other_node) {
      // Copy priors and value to avoid calling evaluator.
      node->expand(*other_node);
      node->update_stats();
      continue;
    }

    // Reached a leaf node.
    auto bp = game_tree.get_path(node);

    // Evaluate leaf node.
    auto pred = evaluator->predict(bp);

    // Expand leaf node.
    node->expand(std::move(pred));

    // Update stats.
    node->update_stats();
  }

  // High level algo:
  // - Make root node for board.
  // - for simulation in simulations
  //   - traverse tree
  //   - if reach a leaf node expand and score
  return SearchResult();
}

} // namespace blunder
