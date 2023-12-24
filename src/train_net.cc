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

  AlphaZeroNet net;

  auto params = net.parameters();
  //for (auto& t : params) {
  //  if (t.dim() >= 2) {
  //    torch::nn::init::xavier_normal_(t);
  //  } else {
  //    torch::nn::init::zeros_(t);
  //  }
  //}

  torch::optim::SGD optimizer(params, /*lr=*/0.02);
  optimizer.zero_grad();

  // Dims are
  // - batch size
  // - number of channels (i.e. depth)
  // - number of columns
  // - rows
  auto t1 = torch::zeros({16, 119, 8, 8}, torch::kCUDA);
  auto t2 = torch::zeros({1, 119, 8, 8}, torch::kCUDA);
  std::cout << "t1.sizes()=" << t1.sizes() << std::endl;
  std::cout << "t2.sizes()=" << t2.sizes() << std::endl;

  std::cout << "Running nets...." << std::endl;

  auto [p1, v1] = net.forward(t1);
  std::cout << "Net finished running for t1 and...\n"
            << "v1.dim() = " << v1.dim()
            << "\nv1.sizes() " << v1.sizes()
            << "\nv1=" << v1
            << "\nv1 -> " << v1.index({0, 0}).item<float>()
            << "\np1.dim() = " << p1.dim()
            << "\np1.sizes() = " << p1.sizes() << std::endl;

  auto [p2, v2] = net.forward(t2);
  std::cout << "Net finished running for t2 and...\n"
            << "v2.dim() = " << v2.dim()
            << "\nv2.sizes() " << v2.sizes()
            << "\nv2=" << v2
            << "\np2.dim() = " << p2.dim()
            << "\np2.sizes() = " << p2.sizes() << std::endl;

  torch::Tensor tensor = torch::full({1}, 3.456789);
  std::cout << "\n\ntensor=" << tensor << std::endl;
  std::cout << "\n\ntensor.dim=" << tensor.dim() << std::endl;

  torch::nn::CrossEntropyLoss ce_loss;
  auto t3 = torch::randn({16, 73, 8, 8}, torch::kCUDA);
  auto t4 = torch::randn({16, 73, 8, 8}, torch::kCUDA);
  auto ce = ce_loss(t3, t4);
  std::cout << "ce.sizes()=" << ce.sizes() << '\n'
            << "ce=" << ce << std::endl;

  return EXIT_SUCCESS;
}
