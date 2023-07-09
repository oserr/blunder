#include <iostream>

#include "net.h"
#include "torch/torch.h"

using namespace blunder;

int
main()
{
  AlphaZeroNet net;

  torch::optim::SGD optimizer(net.parameters(), /*lr=*/0.02);
  optimizer.zero_grad();

  // Dims are
  // - batch size
  // - number of channels (i.e. depth)
  // - number of columns
  // - rows
  auto t = torch::zeros({1, 119, 8, 8});

  std::cout << "Running net...." << std::endl;

  auto [p, v] = net.forward(t);

  std::cout << "Net finished running and...\n"
            << "v.dim() = " << v.dim()
            << "v=" << v << " ...\n"
            << "p.dim() = " << p.dim()
            << "p=....\n" << p << std::endl;

  return EXIT_SUCCESS;
}
