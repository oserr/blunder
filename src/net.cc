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
using ::torch::flatten;
using ::torch::nn::BatchNorm2d;
using ::torch::nn::BatchNorm2dOptions;
using ::torch::nn::Conv2d;
using ::torch::nn::Conv2dOptions;
using ::torch::nn::Linear;
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

bool
clone_params(std::vector<Tensor> from_params, std::vector<Tensor> to_params)
{
  if (from_params.size() != to_params.size())
    return false;

  for (auto [from_tensor, to_tensor] : views::zip(from_params, to_params))
    to_tensor = from_tensor.clone();

  return true;
}

// Loads checkpoint parameters from a file onto a collection of output tensors.
// @file_name The name of the file where the parameters are saved.
// @out_params The output parameters where the checkpoint parameters are loaded
// to. Note that Tensors are like pointers or wrappers in that they don't do
// deep copies by default, so the output tensors point to underlying buffers.
bool
load_params(const fs::path& file_name, std::vector<Tensor> out_params)
{
  if (out_params.empty())
    return false;

  std::vector<Tensor> saved_params;
  torch::load(saved_params, file_name.string());
  return clone_params(std::move(saved_params), std::move(out_params));
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
  out = out + x;
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
  auto out = conv(x);
  out = bnorm(out);
  out = relu(out);
  out = flatten(out, /*start_dim=*/1);
  out = fc1(out);
  out = relu(out);
  out = fc2(out);
  out = tanh(out);
  return out;
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
    std::format_to(std::back_inserter(buff), "res-block-params-{:0>2}.pt", i);
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

  // Turn on inference mode to load the checkpoint.
  c10::InferenceMode inference_mode(true);

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
    std::format_to(std::back_inserter(buff), "res-block-params-{:0>2}.pt", i);
    file_name.replace_filename(buff);
    if (not load_params(file_name.string(), net.parameters()))
      return false;
    buff.clear();
  }

  return true;
}

AlphaZeroNet
AlphaZeroNet::clone() const
{
  AlphaZeroNet other_net;

  if (not clone_params(parameters(), other_net.parameters()))
    throw std::runtime_error("Unable clone input params");

  if (not clone_params(
        policy_net.parameters(), other_net.policy_net.parameters()))
    throw std::runtime_error("Unable clone policy network params.");

  if (not clone_params(
        value_net.parameters(), other_net.value_net.parameters()))
    throw std::runtime_error("Unable clone value network params.");

  for (auto [from, to] : views::zip(res_nets, other_net.res_nets)) {
    if (not clone_params(from.parameters(), to.parameters()))
      throw std::runtime_error("Unable clone residual network params.");
  }
  return other_net;
}

void
AlphaZeroNet::set_eval_mode()
{
  eval();
  policy_net.eval();
  value_net.eval();
  for (auto& res_net : res_nets)
    res_net.eval();
}

void
AlphaZeroNet::set_training_mode()
{
  train();
  policy_net.train();
  value_net.train();
  for (auto& res_net : res_nets)
    res_net.train();
}

} // namespace blunder
