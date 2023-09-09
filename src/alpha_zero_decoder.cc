#include "alpha_zero_decoder.h"

#include <torch/torch.h>

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
  // TODO: add logic to decode the mv tensor into a vector of boards, each
  // representing the next state after a specific move is applied.
  (void)board;
  (void)mv_tensor;
  (void)eval_tensor;
  return DecodedMoves();
}

} // namespace blunder
