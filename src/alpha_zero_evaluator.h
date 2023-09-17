#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include "board_path.h"
#include "evaluator.h"
#include "net.h"
#include "tensor_decoder.h"
#include "tensor_encoder.h"

namespace blunder {

class AlphaZeroEvaluator : public Evaluator {
public:
  AlphaZeroEvaluator(
      std::shared_ptr<AlphaZeroNet> net,
      std::shared_ptr<TensorDecoder> tensor_decoder,
      std::shared_ptr<TensorEncoder> tensor_encoder)
    : net(std::move(net)),
      tensor_decoder(std::move(tensor_decoder)),
      tensor_encoder(std::move(tensor_encoder))
  {
    assert(this->net);
    assert(this->tensor_decoder);
    assert(this->tensor_encoder);
  }

  Prediction
  predict(const BoardPath& board_path) const override;

private:
  std::shared_ptr<AlphaZeroNet> net;
  std::shared_ptr<TensorDecoder> tensor_decoder;
  std::shared_ptr<TensorEncoder> tensor_encoder;
};

} // namespace blunder
