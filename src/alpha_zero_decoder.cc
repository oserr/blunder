#include "alpha_zero_decoder.h"

#include <cmath>
#include <iostream>
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
  assert(mdims.size() == 4);
  assert(mdims[0] == 1 and mdims[1] == 73 and
         mdims[2] == mdims[3] and mdims[2] == 8);

  [[maybe_unused]] auto edims = eval_tensor.sizes();
  assert(edims.size() == 1);

  auto children = board.next();
  if (children.empty()) {
    std::string err = "Processing non-terminal board with no moves...\n";
    err += board.str();
    throw std::runtime_error(std::move(err));
  }

  std::vector<std::pair<Board, float>> move_probs;
  move_probs.reserve(children.size());
  float total = 0;

  auto mtensor = mv_tensor.squeeze();
  for (auto& child : children) {
    auto last_move = child.last_move();
    assert(last_move.has_value());
    auto mv_code = encode_move(*last_move);
    float logit = mtensor.index({mv_code.code, mv_code.row, mv_code.col})
                           .item<float>();
    logit = std::exp(logit);
    total += logit;
    move_probs.emplace_back(std::make_pair(std::move(child), logit));
  }

  for (auto& [ignored, logit] : move_probs)
    logit /= total;

  float value = eval_tensor.index({0}).item<float>();

  return DecodedMoves{
    .move_probs=std::move(move_probs),
    .value=value
  };
}

} // namespace blunder
