#pragma once
#include <vector>
#include "tree/node.h"

class Range {
  private:
    std::vector<double> probs_;
  public:
    Range(int num_info_sets);
    double operator()(int info_set_idx) const;
    std::size_t Size();
};
