//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog_presto.cpp
//
// Identification: src/primer/hyperloglog_presto.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/hyperloglog_presto.h"
#include <bitset>
#include <cstdint>
#include "primer/hyperloglog.h"

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLogPresto<KeyType>::HyperLogLogPresto(int16_t n_leading_bits)
    : dense_bucket_(1 << n_leading_bits, 0), overflow_bucket_(), cardinality_(0), b_(n_leading_bits) {
  if (b_ < 0) {
    cardinality_ = 0;
    return;
  }
}

/** @brief Element is added for HLL calculation. */
template <typename KeyType>
auto HyperLogLogPresto<KeyType>::AddElem(KeyType val) -> void {
  auto hash = CalculateHash(val);
  auto bset = std::bitset<BITSET_CAPACITY>(hash);

  auto leading_zeroes = CalcLeadingZeroes(bset);
  auto idx = CalcIndex(bset);
  SetRegister(idx, leading_zeroes);
}

/** @brief Function to compute cardinality. */
template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  if (b_ < 0) {
    cardinality_ = 0;
    return;
  }
  auto dsum = 0.0;
  for (size_t i = 0; i < dense_bucket_.size(); i++) {
    dsum += pow(2, -static_cast<double>(GetRegisterValue(i)));
  }
  auto m = dense_bucket_.size();
  cardinality_ = static_cast<size_t>(std::floor((CONSTANT * m * m) / dsum));
}

template <typename T>
auto HyperLogLogPresto<T>::CalcIndex(std::bitset<BITSET_CAPACITY> bset) const -> size_t {
  const std::bitset<BITSET_CAPACITY> mask = (~std::bitset<BITSET_CAPACITY>(0)) << (BITSET_CAPACITY - b_);
  std::bitset<BITSET_CAPACITY> extracted = bset & mask;
  extracted >>= (BITSET_CAPACITY - b_);
  return extracted.to_ulong();
}

template <typename T>
auto HyperLogLogPresto<T>::CalcLeadingZeroes(const std::bitset<BITSET_CAPACITY> &bset) const -> uint32_t {
  for (auto i = 0; i < (BITSET_CAPACITY - b_); i++) {
    if (bset[i] == 1) {
      return i;
    }
  }
  return BITSET_CAPACITY - b_;
}

template <typename T>
auto HyperLogLogPresto<T>::CalcOverflow(const std::bitset<TOTAL_BUCKET_SIZE> &bset) const
    -> std::bitset<OVERFLOW_BUCKET_SIZE> {
  const auto mask = (~std::bitset<TOTAL_BUCKET_SIZE>(0)) << DENSE_BUCKET_SIZE;
  auto extracted = bset & mask;
  extracted >>= DENSE_BUCKET_SIZE;
  return std::bitset<OVERFLOW_BUCKET_SIZE>(extracted.to_ulong());
}

template <typename T>
auto HyperLogLogPresto<T>::CalcDense(const std::bitset<TOTAL_BUCKET_SIZE> &bset) const
    -> std::bitset<DENSE_BUCKET_SIZE> {
  const auto mask = (~std::bitset<TOTAL_BUCKET_SIZE>(0)) >> OVERFLOW_BUCKET_SIZE;
  auto extracted = bset & mask;
  return std::bitset<DENSE_BUCKET_SIZE>(extracted.to_ulong());
}

template <typename T>
auto HyperLogLogPresto<T>::SetRegister(uint16_t idx, uint32_t value) -> void {
  auto val_bset = std::bitset<TOTAL_BUCKET_SIZE>(value);
  auto dense_bset = CalcDense(val_bset);
  auto overflow_bset = CalcOverflow(val_bset);

  auto new_val = dense_bset.to_ulong() | (overflow_bset.to_ulong() << DENSE_BUCKET_SIZE);
  auto current_val = GetRegisterValue(idx);

  if (current_val > new_val) {
    return;
  }

  dense_bucket_[idx] = dense_bset;

  if (overflow_bset.to_ulong() > 0) {
    overflow_bucket_[idx] = overflow_bset;
  } else {
    overflow_bucket_.erase(idx);
  }
}

template <typename T>
auto HyperLogLogPresto<T>::GetRegisterValue(uint16_t idx) -> uint32_t {
  auto dense_val = dense_bucket_[idx].to_ulong();
  auto overflow_val = 0UL;
  auto it = overflow_bucket_.find(idx);
  if (it != overflow_bucket_.end()) {
    overflow_val = it->second.to_ulong();
  }
  return dense_val | (overflow_val << DENSE_BUCKET_SIZE);
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub
