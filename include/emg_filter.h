#pragma once

#include "config.h"

class EmgFilter {
 public:
  EmgFilter();
  void setBaseline(float baselineAdc);
  void reset();
  float update(int rawAdc);
  float baseline() const { return baselineAdc_; }

 private:
  float samples_[config::kFilterWindow];
  float sum_;
  float baselineAdc_;
  std::size_t nextIndex_;
  std::size_t count_;
};
