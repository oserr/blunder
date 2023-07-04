#pragma once

#include <iostream>

#include <torch/torch.h>

namespace blunder {

// TODO: all of this is placeholder code that is not really doing anything for
// us, but putting it here as a starting point for developing the neural network
// architecture for training blunder, and to make sure that we can get all of
// this to compile. Note that the code is from pytorch documentation:
// https://pytorch.org/cppdocs/frontend.html.

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

// Placer holder to have code to get things
std::shared_ptr<Net>
train_net();

} // namespace blunder
