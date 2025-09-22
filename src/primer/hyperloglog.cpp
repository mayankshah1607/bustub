//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog.cpp
//
// Identification: src/primer/hyperloglog.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/hyperloglog.h"
#include <cmath>  // Required for std::pow and std::round
#include <cstddef>

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) : cardinality_(0), registers_(1 << n_bits, 0), b_(n_bits) {
  if (b_ < 0 || b_ > BITSET_CAPACITY) {
    cardinality_ = 0;
    return;
  }
}

/**
 * @brief Function that computes binary.
 *
 * @param[in] hash
 * @returns binary of a given hash
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  return std::bitset<BITSET_CAPACITY>(hash);
}

/**
 * @brief Function that computes leading zeros.
 *
 * @param[in] bset - binary values of a given bitset
 * @returns leading zeros of given binary set
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  for (auto i = BITSET_CAPACITY - 1 - b_; i >= 0; i--) {
    if (bset[i]) {
      return BITSET_CAPACITY - b_ - i;
    }
  }
  return 0;
}

/**
 * @brief Adds a value into the HyperLogLog.
 *
 * @param[in] val - value that's added into hyperloglog
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  auto hash = CalculateHash(val);   // compute hash
  auto bset = ComputeBinary(hash);  // convert hash to bitset

  size_t register_idx = CalcIndex(bset);
  auto p = PositionOfLeftmostOne(bset);  // compute position of leftmost one
  registers_[register_idx] = std::max(registers_[register_idx], p);
}

template <typename KeyType>
auto HyperLogLog<KeyType>::CalcIndex(std::bitset<BITSET_CAPACITY> bset) const -> size_t {
  const std::bitset<BITSET_CAPACITY> mask = (~std::bitset<BITSET_CAPACITY>(0)) << (BITSET_CAPACITY - b_);
  std::bitset<BITSET_CAPACITY> extracted = bset & mask;
  extracted >>= (BITSET_CAPACITY - b_);
  return extracted.to_ulong();
}

/**
 * @brief Function that computes cardinality.
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  if (b_ < 0) {
    cardinality_ = 0;
    return;
  }
  auto dsum = 0.0;
  for (size_t i = 0; i < registers_.size(); i++) {
    double term = 1.0 / (1ULL << registers_[i]);
    dsum += term;
  }
  auto m = registers_.size();
  cardinality_ = static_cast<size_t>(std::floor((CONSTANT * m * m) / dsum));
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
