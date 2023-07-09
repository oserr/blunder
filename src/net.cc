#include "net.h"

#include <iostream>

namespace blunder {

namespace {

using ::torch::Tensor;
using ::torch::flatten;
using ::torch::nn::BatchNorm2d;
using ::torch::nn::BatchNorm2dOptions;
using ::torch::nn::Conv2d;
using ::torch::nn::Conv2dOptions;
using ::torch::nn::Linear;
using ::torch::relu;
using ::torch::tanh;

inline Conv2d
make_conv_nn()
{
  auto opts = Conv2dOptions(256, 256, 3).stride(1).padding(1);
  return Conv2d(opts);
}

inline BatchNorm2d
make_bnorm()
{ return BatchNorm2d(BatchNorm2dOptions(256)); }

} // namespace

//---------------
// Residual Block
//---------------

ResBlockNet::ResBlockNet(std::string_view name)
    : conv1(make_conv_nn()),
      conv2(make_conv_nn()),
      bnorm1(make_bnorm()),
      bnorm2(make_bnorm())
{
  register_module(std::format("{}-conv1", name), conv1);
  register_module(std::format("{}-conv2", name), conv2);
  register_module(std::format("{}-bnorm1", name), bnorm1);
  register_module(std::format("{}-bnorm2", name), bnorm2);
}

Tensor
ResBlockNet::forward(Tensor x)
{
  auto out = relu(bnorm1(conv1(x)));
  out = bnorm2(conv2(out));
  out += x;
  return relu(out);
}

//----------------
// Policy head net
//----------------

PolicyNet::PolicyNet()
    : conv1(make_conv_nn()),
      bnorm(make_bnorm()),
      conv2(Conv2d(Conv2dOptions(256, 73, 3).stride(1).padding(1)))
{
  register_module("PolicyNet-conv1", conv1);
  register_module("PolicyNet-bnorm", bnorm);
  register_module("PolicyNet-conv2", conv2);
}

Tensor
PolicyNet::forward(Tensor x)
{ return conv2(relu(bnorm(conv1(x)))); }

//----------------
// Value head net
//----------------

ValueNet::ValueNet()
    : conv(Conv2d(Conv2dOptions(256, 1, 1).stride(1))),
      bnorm(BatchNorm2d(BatchNorm2dOptions(1))),
      fc1(Linear(16, 256)),
      fc2(Linear(256, 1))
{
  register_module("ValueNet-conv", conv);
  register_module("ValueNet-bnorm", bnorm);
  register_module("ValueNet-fc1", fc1);
  register_module("ValueNet-fc2", fc2);
}

Tensor
ValueNet::forward(Tensor x)
{
  auto out = relu(bnorm(conv(x)));
  return tanh(fc2(relu(fc1(flatten(out)))));
}

std::shared_ptr<Net>
train_net()
{
  // Create a new Net.
  auto net = std::make_shared<Net>();

  // Create a multi-threaded data loader for the MNIST dataset.
  auto data_loader = torch::data::make_data_loader(
      torch::data::datasets::MNIST("./data").map(
          torch::data::transforms::Stack<>()),
      /*batch_size=*/64);

  // Instantiate an SGD optimization algorithm to update our Net's parameters.
  torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);

  for (size_t epoch = 1; epoch <= 10; ++epoch) {
    size_t batch_index = 0;
    // Iterate the data loader to yield batches from the dataset.
    for (auto& batch : *data_loader) {
      // Reset gradients.
      optimizer.zero_grad();

      // Execute the model on the input data.
      torch::Tensor prediction = net->forward(batch.data);

      // Compute a loss value to judge the prediction of our model.
      torch::Tensor loss = torch::nll_loss(prediction, batch.target);

      // Compute gradients of the loss w.r.t. the parameters of our model.
      loss.backward();

      // Update the parameters based on the calculated gradients.
      optimizer.step();

      // Output the loss and checkpoint every 100 batches.
      if (++batch_index % 100 == 0) {
        std::cout << "Epoch: " << epoch << " | Batch: " << batch_index
                  << " | Loss: " << loss.item<float>() << std::endl;

        // Serialize your model periodically as a checkpoint.
        torch::save(net, "net.pt");
      }
    }
  }

  return net;
}

} // namespace blunder
