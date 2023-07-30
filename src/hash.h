#pragma once

#include <functional>

namespace blunder {

// Inspired by:
// - https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x.
// - https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes

template<typename T>
inline size_t
compute_hash(size_t seed, const T& val) noexcept
{
  std::hash<T> hasher;
  return hasher(val) + 0x517cc1b727220a95ull + (seed << 6) + (seed >> 2);
}

template <typename T, typename... Rest>
inline size_t
compute_hash(size_t seed, const T& val, const Rest&... rest) noexcept
{
  seed = compute_hash(seed, val);
  return seed ^ compute_hash(seed, rest...);
}

template <typename T, typename... Rest>
inline std::size_t
combine_hash(const T& val, const Rest&... rest) noexcept
{ return compute_hash(0, val, rest...); }

} // namespace blunder
