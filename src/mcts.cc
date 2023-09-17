#include "mcts.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
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
  // The initial value from the network or from a terminal state.
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

  // Returns a board path from the current node, with current node the root
  // node of the board path.
  BoardPath
  get_path(const BoardPath& from_root) const noexcept;

  // Returns true if the board reached a terminal state.
  bool
  is_terminal() const noexcept
  { return board.is_terminal(); }

  // Terminates the node by setting a value based on whether player making the
  // move is winning or not, like expand, but without computing subsequent
  // actions.
  Node&
  terminate();

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

  // Adds noise to the priors.
  void
  add_noise() noexcept;
};

BoardPath
Node::get_path(const BoardPath& from_root) const noexcept
{
  auto node = this;
  BoardPath board_path;

  // Check for parent to avoid adding the root node here, since push(from_root)
  // takes care of adding the root.
  while (node and node->parent and not board_path.is_full()) {
    board_path.push(node->board);
    node = node->parent;
  }

  board_path.push(from_root);

  return board_path;
}

Node&
Node::expand(Prediction pred)
{
  assert(is_leaf and not is_terminal());

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

Node&
Node::terminate()
{
  if (not board.is_terminal())
    throw std::logic_error("Node is not in a terminal state.");

  // The value of 1 here is for the move leading up to the check.
  init_value = board.is_mate() ? 1.0 : 0.0;
  value = value;
  return *this;
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

} // namespace

Mcts::Mcts(
    std::shared_ptr<Evaluator> evaluator,
    unsigned simulations,
    unsigned seed)
    : evaluator(std::move(evaluator)),
      simuls(simulations)
{
  std::mt19937 gen(seed);
  std::gamma_distribution<float> dir(DIR_ALPHA);
  dir_fn = std::bind_front(std::move(dir), std::move(gen));
}

// TODO: Determine if adding some sort of caching improves performance.
SearchResult
Mcts::run(const BoardPath& board_path) const
{
  auto board = board_path.root();
  if (not board)
    throw std::invalid_argument("BoardPath should have a root.");

  auto pred = evaluator->predict(board_path);

  // Copy the priors before adding noise to them.
  SearchResult result;
  result.moves.reserve(pred.move_probs.size());
  for (const auto& [board, prior] : pred.move_probs)
    result.moves.emplace_back(BoardProb{.board=board, .prior=prior});

  add_noise(pred.move_probs);

  Node root(board->get());
  root.expand(pred);

  for (unsigned i = 0; i < simuls; ++i) {
    auto* node = &root;

    while (not node->is_leaf and not node->is_terminal()) {
      node = node->choose_next();
      assert(node);
    }

    // We reached a terminal state so there is no need to call evaluator.
    if (node->is_terminal()) {
      node->terminate().update_stats();
      continue;
    }

    // Reached a leaf node.
    auto bp = node->get_path(board_path);

    // Evaluate leaf node.
    auto pred = evaluator->predict(bp);

    // Expand leaf node.
    node->expand(std::move(pred)).update_stats();
  }

  const Node* max_node = nullptr;
  unsigned max_visits = 0;
  unsigned i = 0;
  for (const auto& child : root.children) {
    auto visits = child.visits;
    result.moves[i++].visits = visits;
    if (visits > max_visits) {
      max_visits = visits;
      max_node = &child;
    }
  }

  if (not max_node)
    throw std::runtime_error(
        "The MCTS should only run for boards with a non-terminal state.");

  result.best.board = max_node->board;
  result.best.prior = max_node->prior;
  result.best.visits = max_node->visits;

  result.value = max_node->init_value;

  return result;
}

// Adds noise to the root node's priors to encourage exploration.
void
Mcts::add_noise(std::span<std::pair<Board, float>> priors) const
{
  for (auto& [ignored, prior] : priors) {
    auto noise = dir_fn();
    auto term1 = prior * (1 - DIR_EXPLORE_FRAC);
    auto term2 = noise * DIR_EXPLORE_FRAC;
    prior = term1 + term2;
  }
}

} // namespace blunder
