#include "alpha_zero_decoder.h"

#include <cmath>
#include <stdexcept>
#include <torch/torch.h>

#include "coding_util.h"

namespace blunder {

// TODO: Add logic to decode policy representation for moves given a position,
// which uses an 8x8x73 stack of planes to encode policy, where the 1st 56
// planes encode possible queen moves for any piece, the number of squares
// [1..7] in which the piece will be moved, along one of eight relative compass
// directions {N, NE, E, SE, S, SW, W, NW}. The next 8 planes encode possible
// knight moves. The final 9 planes encoded possible underpromotions for pawn
// moves or captures in two possible diagonals, to knight, bishop, or rook
// respectively. Other pawn moves or captures from the 7th rank are promoted to
// a queen.
//
// This can be represented by an (8, 8, 73) tensor, where the the first
// dimension represents a column, the second the row, and the third is one of 73
// numbers representing a type of move. For example, for the first 56 moves, to
// represent possible queen moves for any piece, we can do something like the
// following:
// - 1N, 1NE, 1E,..., 2N, 2NE, 2E, ..., 7N, 7NE, 7E,... and so on.

DecodedMoves
AlphaZeroDecoder::decode(
    const Board& board,
    const torch::Tensor& mv_tensor,
    const torch::Tensor& eval_tensor) const
{
  assert(not board.is_terminal());
  [[maybe_unused]] auto mdims = mv_tensor.sizes();
  assert(mdims.size() == 3);
  assert(mdims[0] == mdims[1] and mdims[1] == 8 and mdims[2] == 73);

  [[maybe_unused]] auto edims = eval_tensor.sizes();
  assert(edims.size() == 2 and edims[0] == edims[1] and edims[0] == 1);

  auto children = board.next();
  if (children.empty())
    throw std::runtime_error("Decoding only works with a non-terminal board.");

  std::vector<std::pair<Board, float>> move_probs;
  move_probs.reserve(children.size());
  float total = 0;

  for (auto& child : children) {
    auto last_move = child.last_move();
    assert(last_move.has_value());
    auto mv_code = encode_move(*last_move);
    float logit = mv_tensor.index({mv_code.row, mv_code.col, mv_code.code})
                           .item<float>();
    logit = std::exp(logit);
    total += logit;
    move_probs.emplace_back(std::make_pair(std::move(child), logit));
  }

  for (auto& [ignored, logit] : move_probs)
    logit /= total;

  float value = eval_tensor.index({0, 0}).item<float>();

  return DecodedMoves{
    .move_probs=std::move(move_probs),
    .value=value
  };
}

} // namespace blunder
