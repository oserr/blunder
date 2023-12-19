#include "net.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <iterator>
#include <ranges>
#include <system_error>

namespace blunder {

namespace {

using ::torch::Tensor;
using ::torch::data::datasets::Dataset;
using ::torch::data::Example;
using ::torch::flatten;
using ::torch::nn::BatchNorm2d;
using ::torch::nn::BatchNorm2dOptions;
using ::torch::nn::Conv2d;
using ::torch::nn::Conv2dOptions;
using ::torch::nn::Linear;
using ::torch::nn::MSELoss;
using ::torch::nn::CrossEntropyLoss;
using ::torch::relu;
using ::torch::tanh;

namespace fs = std::filesystem;
namespace views = std::ranges::views;

inline Conv2d
make_conv_nn()
{
  auto opts = Conv2dOptions(256, 256, 3).stride(1).padding(1);
  return Conv2d(opts);
}

inline BatchNorm2d
make_bnorm()
{ return BatchNorm2d(BatchNorm2dOptions(256)); }

using TensorPair = std::pair<Tensor, Tensor>;

// TODO: When we begin to generate training data through self play, we'll save
// the game history, and this custom loader will need to read the data and
// convert it to tensors.
//
// Placer holder code for a custom dataset to use with a dataloader.
// Each example will consists of the input to the network, i.e. an 8x8x119 stack
// of planes representing the current chess position, and the target will
// consist of a pair of tensors, one to represent the value and the other to
// represent the move policy.
class ChessDataset :
  public Dataset<ChessDataset, Example<Tensor, TensorPair>>
{
public:
  //static constexpr bool is_stateful = false;

  ExampleType
  get(size_t) override
  { return ExampleType(); }

  torch::optional<size_t>
  size() const override
  { return 0; };
};

// Loads checkpoint parameters from a file onto a collection of output tensors.
// @file_name The name of the file where the parameters are saved.
// @out_params The output parameters where the checkpoint parameters are loaded
// to. Note that Tensors are like pointers or wrappers in that they don't do
// deep copies by default, so the output tensors point to underlying buffers.
bool
load_params(const fs::path& file_name, std::vector<torch::Tensor> out_params)
{
  if (out_params.empty())
    return false;

  std::vector<torch::Tensor> saved_params;
  torch::load(saved_params, file_name.string());
  if (saved_params.size() != out_params.size())
    return false;

  for (auto [saved_tensor, out_tensor] : views::zip(saved_params, out_params))
    out_tensor.copy_(saved_tensor);

  return true;
}

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
  this->to(torch::kCUDA);
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
  this->to(torch::kCUDA);
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
      fc1(Linear(64, 256)),
      fc2(Linear(256, 1))
{
  register_module("ValueNet-conv", conv);
  register_module("ValueNet-bnorm", bnorm);
  register_module("ValueNet-fc1", fc1);
  register_module("ValueNet-fc2", fc2);
  this->to(torch::kCUDA);
}

Tensor
ValueNet::forward(Tensor x)
{
  auto out = relu(bnorm(conv(x)));
  return tanh(fc2(relu(fc1(flatten(out)))));
}

//--------------
// AlphaZero net
//--------------

AlphaZeroNet::AlphaZeroNet()
  : conv(Conv2d(Conv2dOptions(119, 256, 3).stride(1).padding(1))),
    bnorm(make_bnorm()),
    policy_net(),
    value_net()
{
  register_module("input-conv", conv);
  register_module("input-bnorm", bnorm);

  // Initialize the residual blocks.
  res_nets.reserve(19);
  for (int i = 0; i < 19; ++i)
    res_nets.emplace_back(std::format("ResNetBlock-{}", i));

  this->to(torch::kCUDA);
}

std::pair<Tensor, Tensor>
AlphaZeroNet::forward(Tensor x)
{
  auto out = conv(x);
  out = bnorm(out);
  out = relu(out);

  for (auto& res_net : res_nets)
    out = res_net.forward(out);

  auto pol = policy_net.forward(out);
  auto val = value_net.forward(out);

  return {pol, val};
}

void
AlphaZeroNet::on_device(torch::Device device)
{
  policy_net.to(device);
  value_net.to(device);

  for (auto& res_net: res_nets)
    res_net.to(device);

  to(device);
}

bool
AlphaZeroNet::create_checkpoint(const fs::path& checkpoint_dir)
{
  // Check that the dir does not exist or that it is empty.
  if (fs::exists(checkpoint_dir) &&
      (not fs::is_directory(checkpoint_dir) || not fs::is_empty(checkpoint_dir)))
    return false;

  std::error_code err;
  fs::create_directories(checkpoint_dir, err);
  if (err) return false;

  // Turn on inference mode to create the checkpoint.
  c10::InferenceMode inference_mode(true);

  auto fpath = checkpoint_dir;

  // Save the input params.
  fpath.append("input-params.pt");
  torch::save(parameters(), fpath.string());

  // Save the policy head params.
  fpath.replace_filename("policy-params.pt");
  torch::save(policy_net.parameters(), fpath.string());

  // Save the policy head params.
  fpath.replace_filename("value-params.pt");
  torch::save(value_net.parameters(), fpath.string());

  // Save the residual block parameters.
  std::string buff;
  buff.reserve(32);
  for (const auto [i, net] : views::enumerate(res_nets)) {
    std::format_to(std::back_inserter(buff), "res-block-params-{}.pt", i);
    fpath.replace_filename(buff);
    torch::save(net.parameters(), fpath.string());
    buff.clear();
  }

  return true;
}

bool
AlphaZeroNet::load_checkpoint(const fs::path& checkpoint_dir)
{
  // Check that the dir exists.
  if (not fs::exists(checkpoint_dir) || not fs::is_directory(checkpoint_dir))
    return false;

  auto file_name = checkpoint_dir;

  file_name.append("input-params.pt");
  if (not load_params(file_name, parameters()))
    return false;

  file_name.replace_filename("policy-params.pt");
  if (not load_params(file_name, policy_net.parameters()))
    return false;

  file_name.replace_filename("value-params.pt");
  if (not load_params(file_name, value_net.parameters()))
    return false;

  std::string buff;
  buff.reserve(32);
  for (const auto [i, net] : views::enumerate(res_nets)) {
    std::format_to(std::back_inserter(buff), "res-block-params-{}.pt", i);
    file_name.replace_filename(buff);
    if (not load_params(file_name.string(), net.parameters()))
        return false;
    buff.clear();
  }

  return true;
}

std::shared_ptr<AlphaZeroNet>
train_net()
{
  // Create a new Net.
  auto net = std::make_shared<AlphaZeroNet>();

  // Create a multi-threaded data loader for the MNIST dataset.
  auto data_loader = torch::data::make_data_loader(
      //torch::data::datasets::MNIST("./data").map(
      //    torch::data::transforms::Stack<>()),
      ChessDataset(),
      /*batch_size=*/64);

  // Instantiate an SGD optimization algorithm to update our Net's parameters.
  torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);

  for (size_t epoch = 1; epoch <= 10; ++epoch) {
    size_t batch_index = 0;
    // Iterate the data loader to yield batches from the dataset.
    for (auto& batch : *data_loader) {
      // TODO: define a function to stack Elements when we create the data
      // loader, so we can operate on the whole batch rather than individual
      // examples. For now do this to get it to compile.
      for (auto& example : batch) {
        // Reset gradients.
        optimizer.zero_grad();

        // Execute the model on the input data.
        auto [policy_pred, value_pred] = net->forward(example.data);

        // Get the target values from self play.
        auto& [policy_target, value_target] = example.target;

        // TODO: compute loss for value
        MSELoss mse_loss;
        auto value_loss = mse_loss(value_pred, value_target);

        CrossEntropyLoss ce_loss;
        auto policy_loss = ce_loss(policy_pred, policy_target);

        // TODO: add L2 regularization.
        auto loss = value_loss + policy_loss;
        loss.backward();

        // Update the parameters based on the calculated gradients.
        optimizer.step();

        // Output the loss and checkpoint every 100 batches.
        if (++batch_index % 100 == 0) {
          std::cout << "Epoch: " << epoch
                    << " | Batch: " << batch_index
                    << " | Loss: " << loss.item<float>()
                    << std::endl;

          // Serialize your model periodically as a checkpoint.
          torch::save(net, "net.pt");
        }
      }
    }
  }

  return net;
}

} // namespace blunder
