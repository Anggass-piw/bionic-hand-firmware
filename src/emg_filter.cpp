#include "emg_filter.h"

#include <cmath>

EmgFilter::EmgFilter()
    : sumMagnitude_(0.0f),
      sumSquares_(0.0f),
      baselineAdc_(0.0f),
      nextIndex_(0),
      count_(0) {
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
  sumMagnitude_ = 0.0f;
  sumSquares_ = 0.0f;
  nextIndex_ = 0;
  count_ = 0;
}

float EmgFilter::update(int rawAdc) {
  // Center around the measured relaxed baseline, then rectify: |x[n]|.
  const float magnitude = std::fabs(static_cast<float>(rawAdc) - baselineAdc_);

  if (count_ == config::kFilterWindow) {
    const float outgoing = samples_[nextIndex_];
    sumMagnitude_ -= outgoing;
    sumSquares_ -= outgoing * outgoing;
  } else {
    ++count_;
  }

  samples_[nextIndex_] = magnitude;
  sumMagnitude_ += magnitude;
  sumSquares_ += magnitude * magnitude;
  nextIndex_ = (nextIndex_ + 1) % config::kFilterWindow;
  return config::kUseRmsFeature ? rms() : mav();
}

float EmgFilter::mav() const {
  return count_ == 0 ? 0.0f : sumMagnitude_ / static_cast<float>(count_);
}

float EmgFilter::rms() const {
  if (count_ == 0) return 0.0f;
  const float meanSquare = sumSquares_ / static_cast<float>(count_);
  return meanSquare > 0.0f ? std::sqrt(meanSquare) : 0.0f;
}
