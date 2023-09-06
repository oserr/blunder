#include "alpha_zero_encoder.h"

#include <cassert>
#include <initializer_list>
#include <stdexcept>

#include "board.h"
#include "board_path.h"
#include "piece_set.h"

namespace tix = torch::indexing;

namespace blunder {
namespace {

// Returns the row and column for a square.
// TODO: Put this in a more generic place.
inline std::pair<int, int>
row_col(unsigned square) noexcept
{ return {square / 8, square % 8}; }

// Encodes the bitboard pieces in the planes of the tensor.
void
encode_pieces(int plane, const PieceSet& pieces, torch::Tensor& tensor) {
  [[maybe_unused]] auto dims = tensor.sizes();
  assert(dims.size() == 3);
  assert(dims[1] == dims[2] and dims[2] == 8);
  assert(plane >= 0 and plane < dims[0]);
  for (auto piece : pieces) {
    for (auto square : piece.square_iter()) {
      auto [row, col] = row_col(square);
      tensor.index_put_({plane++, row, col}, 1.0);
    }
  }
}

} // namespace

torch::Tensor
AlphaZeroEncoder::encode(const BoardPath& board_path) const
{
  auto root_board = board_path.root();
  if (not root_board)
    throw std::invalid_argument("board_path should not be empty.");

  const Board& root = *root_board;

  // TODO: Add more fine grained control to allow creating the tensor on the GPU
  // device with grad enabled. For now, this creates a strided-float-CPU tensor
  // with grad disabled.
  //
  // See https://pytorch.org/cppdocs/notes/tensor_creation.html.
  auto tensor = torch::zeros({119, 8, 8});
  int plane = 0;

  if (root.is_white_next()) {
    for (const auto& board : board_path) {
      auto [white, black] = board.white_black();

      encode_pieces(plane, *white, tensor);
      plane += 6;

      encode_pieces(plane, *black, tensor);
      plane += 6;

      // TODO: Set repetition planes for each board.
      plane += 2;
    }
  } else {
    // Here we need to flip the boards to orient them from the perspective of
    // black.
    for (const auto& board : board_path) {
      auto [white, black] = board.white_black();

      encode_pieces(plane, white->flip(), tensor);
      plane += 6;

      encode_pieces(plane, black->flip(), tensor);
      plane += 6;

      // TODO: Set repetition planes for each board.
      plane += 2;
    }
  }

  const unsigned bin_features[7] = {
    root.is_white_next(),
    root.fm_count(),
    root.has_wk_castling(),
    root.has_wq_castling(),
    root.has_bk_castling(),
    root.has_bq_castling(),
    root.hm_count()
  };

  for (auto bin_feat : bin_features) {
    if (bin_feat) {
      auto index = std::initializer_list<tix::TensorIndex>{
        plane, tix::Slice(), tix::Slice()};
      tensor.index_put_(index, static_cast<float>(bin_feat));
    }
    ++plane;
  }

  return tensor;
}

} // namespace blunder