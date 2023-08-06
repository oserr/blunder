#pragma once

#include <array>

#include "board.h"

namespace blunder {

// A class with array-like semantics to represent a path in a game tree. The use
// case for this class are as follows:
// - to use as input for the evaluator, where we expect a series of boards for
//   multiple time steps, but in reverse order, i.e. board at time step T goes
//   before board at time step T-1, and board at time step 0 is last board.
// - To put togheter multiple boards contiguously when they are not already
//   contiguous, like during game tree search, where the underlying data
//   structure is a tree.
// - For now, the max number of boards in the path is 8 since that is how many
//   timesteps are used for evaluation, but this can be adjusted later if there
//   is a need.
class BoardPath {
  BoardPath() noexcept = default;

  bool
  is_full() const noexcept
  { return n == boards.size(); }

  // Pushes a board onto the board path if not full yet.
  void
  push(const Board& board) noexcept
  {
    if (is_full())
      return;
    boards[n++] = &board;
  }

  // Pushes as many boards from other as can fit in the array.
  void
  push(const BoardPath& other) noexcept
  {
    for (const auto& board : other) {
      if (is_full())
        break;
      boards[n++] = &board;
    }
  }

private:
  using BoardPathArr = std::array<const Board*, 8>;

  // An iterator helper class to make it easier to traverse boards in BoardPath.
  class Iter {
  public:
    using iterator = BoardPathArr::const_iterator;

    Iter(const Iter& iter) = default;
    Iter(iterator iter)
     : iter(iter) {}

    const Board&
    operator*() const noexcept
    { return **iter; }

    // Prefix ++.
    Iter&
    operator++() noexcept
    {
      ++iter;
      return *this;
    }

    // Postfix ++.
    Iter
    operator++(int) noexcept
    {
      Iter current(*this);
      ++iter;
      return current;
    }

    // Equality operators.

    bool
    operator==(Iter other) const noexcept
    { return iter == other.iter; }

    bool
    operator!=(Iter other) const noexcept
    { return iter != other.iter; }

  private:
    iterator iter;
  };


public:
  // Returns an iterator to the first board if one is available.
  Iter
  begin() const noexcept
  { return Iter(boards.cbegin()); }

  // Returns an iterator the end. If there are no boards available, then this is
  // equal to the iterator from begin.
  Iter
  end() const noexcept
  { return Iter(boards.cbegin() + n); }

private:

  BoardPathArr boards;

  // The number of boards.
  unsigned n = 0;
};

} // namespace blunder
