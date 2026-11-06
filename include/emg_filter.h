#pragma once

#include "config.h"

class EmgFilter {
 public:
  EmgFilter();
  void setBaseline(float baselineAdc);
  void reset();
  float update(int rawAdc);
  float mav() const;
  float rms() const;
  float baseline() const { return baselineAdc_; }

 private:
  float samples_[config::kFilterWindow];
  float sumMagnitude_;
  float sumSquares_;
  float baselineAdc_;
  std::size_t nextIndex_;
  std::size_t count_;
};
