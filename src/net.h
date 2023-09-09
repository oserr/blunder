#pragma once

#include <string_view>
#include <utility>
#include <vector>

#include <torch/torch.h>

namespace blunder {

// TODO: all of this is placeholder code that is not really doing anything for
// us, but putting it here as a starting point for developing the neural network
// architecture for training blunder, and to make sure that we can get all of
// this to compile. Note that the code is from pytorch documentation:
// https://pytorch.org/cppdocs/frontend.html.

// TODO: implement logic to encode the input feature for the network, which is 6
// planes for each of the pieces for each player, i.e. 12 total planes, and 2
// planes for repetition count in current position for each player. These are
// repeated for 8 time steps starting from the current position. There are an
// additional 7 layers for
//
// - 1 for piece color
// - 1 for total move count
// - 2 for king and queenside castling for the current player
// - 2 for castling for the other player
// - no progress count (50 move rule)
//
// In total, the input feature consists of 119 (8, 8) feature planes.

// TODO: implement AlphaZero neural architecture:
// 1) An initial convolution
//   - A convolution of 256 filters of kernel size 3x3 with stride 1.
//   - batch normalization
//   - a rectifier nonlinearity
// 2) 19 residual blocks, each consisting of the following
//   - A convolution of 256 filters of kernel size 3x3 with stride 1.
//   - batch normalization
//   - a rectifier nonlinearity
//   - A convolution of 256 filters of kernel size 3x3 with stride 1.
//   - batch normalization
//   - a skip connection that adds the input to the block
//   - a rectifier nonlinearity
// 3) the output is then passed to two heads
//    A) Policy head
//      - A convolution of 256 filters of kernel size 3x3 with stride 1.
//      - batch normalization
//      - a rectifier nonlinearity
//      - convolution of 73 filters
//    B) Value head
//      - A convolution of 1 filter of kernel size 1x1 with stride 1.
//      - batch normalization
//      - a rectifier nonlinearity
//      - a fully connected linear layer to a hidden layer of size 256
//      - a rectified non linearity
//      - a fully connected layer to a scalar
//      - a tanh nonlinearity outputting a scalar in the range [-1, 1].

//---------------
// Residual Block
//---------------

// ResBlockNet implements the residual block in the AlphaZero network, which has
// 19 of these blocks connected together.
struct ResBlockNet : torch::nn::Module {
  explicit
  ResBlockNet(std::string_view name);

  torch::Tensor
  forward(torch::Tensor x);

  torch::nn::Conv2d conv1 = nullptr;
  torch::nn::Conv2d conv2 = nullptr;
  torch::nn::BatchNorm2d bnorm1 = nullptr;
  torch::nn::BatchNorm2d bnorm2 = nullptr;
};

//----------------
// Policy head net
//----------------

struct PolicyNet : torch::nn::Module {
  PolicyNet();

  torch::Tensor
  forward(torch::Tensor x);

  torch::nn::Conv2d conv1 = nullptr;
  torch::nn::BatchNorm2d bnorm = nullptr;
  torch::nn::Conv2d conv2 = nullptr;
};

//----------------
// Value head net
//----------------

struct ValueNet : torch::nn::Module {
  ValueNet();

  torch::Tensor
  forward(torch::Tensor x);

  torch::nn::Conv2d conv = nullptr;
  torch::nn::BatchNorm2d bnorm = nullptr;
  torch::nn::Linear fc1 = nullptr;
  torch::nn::Linear fc2 = nullptr;
};

//--------------
// AlphaZero net
//--------------

// TODO: figure out how to enable the network to operate in inference mode to
// make computations more efficient:
// https://pytorch.org/cppdocs/notes/inference_mode.html.
struct AlphaZeroNet : torch::nn::Module {
  AlphaZeroNet();

  std::pair<torch::Tensor, torch::Tensor>
  forward(torch::Tensor x);

  torch::nn::Conv2d conv = nullptr;
  torch::nn::BatchNorm2d bnorm = nullptr;
  PolicyNet policy_net;
  ValueNet value_net;
  std::vector<ResBlockNet> res_nets;
};

// TODO: implement a training loop.
std::shared_ptr<AlphaZeroNet>
train_net();

} // namespace blunder
