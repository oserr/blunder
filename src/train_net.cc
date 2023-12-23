#include <algorithm>
#include <iostream>
#include <format>
#include <vector>

#include "net.h"
#include "torch/torch.h"

using namespace blunder;

int
main()
{
  c10::InferenceMode inference_mode;
  for (int i = 0; i < 20; ++i)
    std::cout << std::format("{:0>2}\n", i);;

  AlphaZeroNet net;
  if (net.create_checkpoint("checkpoint")) {
    std::cout << "checkpoint created successfully" << std::endl;
  } else {
    std::cout << "unable to create checkpoint" << std::endl;
  }

  AlphaZeroNet other_net;
  if (other_net.load_checkpoint("checkpoint")) {
    std::cout << "unable to load checkpoint" << std::endl;
  } else {
    std::cout << "checkpoint loaded successfully" << std::endl;
  }

  auto params = net.parameters();
  for (auto& t : params) {
    if (t.dim() >= 2) {
      torch::nn::init::xavier_normal_(t);
    } else {
      torch::nn::init::zeros_(t);
    }
  }

  torch::optim::SGD optimizer(params, /*lr=*/0.02);
  optimizer.zero_grad();

  // Dims are
  // - batch size
  // - number of channels (i.e. depth)
  // - number of columns
  // - rows
  auto t = torch::zeros({1, 119, 8, 8}).to(torch::kCUDA);

  std::cout << "Running net...." << std::endl;

  auto [p, v] = net.forward(t);

  std::cout << "Net finished running and...\n"
            << "v.dim() = " << v.dim()
            << "\nv=" << v
            << "\np.dim() = " << p.dim() << std::endl;

  torch::Tensor tensor = torch::full({1}, 3.456789);
  std::cout << "\n\ntensor=" << tensor << std::endl;
  std::cout << "\n\ntensor.dim=" << tensor.dim() << std::endl;

  return EXIT_SUCCESS;
}
