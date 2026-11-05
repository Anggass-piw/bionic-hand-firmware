#include "emg_filter.h"

#include <cmath>

EmgFilter::EmgFilter()
    : sum_(0.0f), baselineAdc_(0.0f), nextIndex_(0), count_(0) {
  reset();
}

void EmgFilter::setBaseline(float baselineAdc) {
  baselineAdc_ = baselineAdc;
  reset();
}

void EmgFilter::reset() {
  for (std::size_t i = 0; i < config::kFilterWindow; ++i) {
    samples_[i] = 0.0f;
  }
  sum_ = 0.0f;
  nextIndex_ = 0;
  count_ = 0;
}

float EmgFilter::update(int rawAdc) {
  // Rectify around the measured relaxed baseline. This works with conditioned
  // bipolar EMG as well as envelope-style sensor outputs after baseline removal.
  const float magnitude = std::fabs(static_cast<float>(rawAdc) - baselineAdc_);

  if (count_ == config::kFilterWindow) {
    sum_ -= samples_[nextIndex_];
  } else {
    ++count_;
  }

  samples_[nextIndex_] = magnitude;
  sum_ += magnitude;
  nextIndex_ = (nextIndex_ + 1) % config::kFilterWindow;
  return count_ == 0 ? 0.0f : sum_ / static_cast<float>(count_);
}
