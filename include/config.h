#pragma once

#include <cstddef>
#include <cstdint>

namespace config {
// ESP32 DevKit: GPIO34 is an ADC1 input-only pin; GPIO18 drives the servo signal.
constexpr uint8_t kEmgPin = 34;
constexpr uint8_t kServoPin = 18;

constexpr uint32_t kAdcMax = 4095;
constexpr uint16_t kFilterWindow = 25;
constexpr uint16_t kSampleIntervalMs = 2;       // 500 Hz sampling target
constexpr uint16_t kServoUpdateIntervalMs = 20; // 50 Hz servo updates
constexpr uint8_t kServoStepDegrees = 2;
constexpr uint16_t kServoFrequencyHz = 50;
constexpr uint16_t kServoPulseMinUs = 500;
constexpr uint16_t kServoPulseMaxUs = 2400;

// Keep these conservative and tune only after bench-testing the mechanism.
constexpr int kServoOpenDeg = 10;
constexpr int kServoGripDeg = 95;

constexpr uint32_t kCalibrationDurationMs = 3000;
constexpr float kPeakAcceptanceFloorAdc = 30.0f;
constexpr float kThresholdFloorAdc = 12.0f;
constexpr float kThresholdFractionOfPeak = 0.20f;
constexpr float kReleaseFractionOfThreshold = 0.70f;
constexpr uint16_t kTelemetryIntervalMs = 100;
}  // namespace config
