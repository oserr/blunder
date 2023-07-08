#pragma once

#include <iostream>

#include <torch/torch.h>

namespace blunder {

// TODO: all of this is placeholder code that is not really doing anything for
// us, but putting it here as a starting point for developing the neural network
// architecture for training blunder, and to make sure that we can get all of
// this to compile. Note that the code is from pytorch documentation:
// https://pytorch.org/cppdocs/frontend.html.

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
// numbers representing a type of move. For example, the, for the first 56
// moves, to represent possible queen moves for any piece, we can do something
// like the following:
// - 1N, 1NE, 1E,..., 2N, 2NE, 2E, ..., 7N, 7NE, 7E,... and so on.

// TODO: implement logic to encode the input feature for the network, which is 6
// planes for each of the pieces for each player, i.e. 12 total planes, and 2
// planes for repetition count in current position for each player. These are
// repeated for 8 time steps up to the current position. There are an additional
// 7 layers for
//
// - 1 for piece color
// - 1 for total move count
// - 2 for king and queenside castling for the current player
// - 2 for castling for the other player
// - no progress count (50 move rule)
//
// In total, the input feature consists of 119 (8, 8) feature planes.

// Define a new Module.
struct Net : torch::nn::Module {
  Net()
    : fc1(register_module("fc1", torch::nn::Linear(784, 64))),
      fc2(register_module("fc2", torch::nn::Linear(64, 32))),
      fc3(register_module("fc3", torch::nn::Linear(32, 10)))
  {}

  // Implement the Net's algorithm.
  torch::Tensor
  forward(torch::Tensor x)
  {
    x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
    x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
    x = torch::relu(fc2->forward(x));
    x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
    return x;
  }

  torch::nn::Linear fc1{nullptr};
  torch::nn::Linear fc2{nullptr};
  torch::nn::Linear fc3{nullptr};
};

// TODO: implement a training loop.
std::shared_ptr<Net>
train_net();

} // namespace blunder
