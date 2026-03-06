#include "solver/range.h"

Range::Range(int num_info_sets)
  : probs_(num_info_sets, 1.0 / num_info_sets) {}

double Range::operator()(int info_set_idx) const {
  return probs_[info_set_idx];
}

std::size_t Range::Size() {
  return probs_.size();
}
