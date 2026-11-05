#include "signal_logic.h"

#include <cmath>

int mapSignalToServo(float signal, float threshold, float peak,
                     int openAngle, int gripAngle) {
  if (peak <= threshold || signal <= threshold) {
    return openAngle;
  }

  float ratio = (signal - threshold) / (peak - threshold);
  if (ratio < 0.0f) ratio = 0.0f;
  if (ratio > 1.0f) ratio = 1.0f;

  const float angle = static_cast<float>(openAngle) +
                      ratio * static_cast<float>(gripAngle - openAngle);
  return static_cast<int>(std::lround(angle));
}

bool updateContractionState(float signal, float onsetThreshold,
                            float releaseThreshold, bool wasActive) {
  if (wasActive) {
    return signal > releaseThreshold;
  }
  return signal >= onsetThreshold;
}
