#include "servo_handler.h"

#include <Arduino.h>

#include "config.h"

ServoHandler::ServoHandler()
    : currentAngle_(config::kServoOpenDeg),
      targetAngle_(config::kServoOpenDeg),
      lastUpdateMs_(0) {}

void ServoHandler::begin(int pin, int initialAngle) {
  currentAngle_ = initialAngle;
  targetAngle_ = initialAngle;
  servo_.setPeriodHertz(config::kServoFrequencyHz);
  servo_.attach(pin, config::kServoPulseMinUs, config::kServoPulseMaxUs);
  servo_.write(currentAngle_);
  lastUpdateMs_ = millis();
}

void ServoHandler::setTarget(int angle) {
  const int lower = config::kServoOpenDeg < config::kServoGripDeg
                        ? config::kServoOpenDeg
                        : config::kServoGripDeg;
  const int upper = config::kServoOpenDeg > config::kServoGripDeg
                        ? config::kServoOpenDeg
                        : config::kServoGripDeg;
  if (angle < lower) angle = lower;
  if (angle > upper) angle = upper;
  targetAngle_ = angle;
}

void ServoHandler::update() {
  if (!servo_.attached()) return;

  const uint32_t now = millis();
  if (now - lastUpdateMs_ < config::kServoUpdateIntervalMs) return;
  lastUpdateMs_ = now;

  const int difference = targetAngle_ - currentAngle_;
  if (difference == 0) return;

  const int step = difference > 0 ? config::kServoStepDegrees
                                 : -config::kServoStepDegrees;
  if (difference > 0 && difference < config::kServoStepDegrees) {
    currentAngle_ = targetAngle_;
  } else if (difference < 0 && -difference < config::kServoStepDegrees) {
    currentAngle_ = targetAngle_;
  } else {
    currentAngle_ += step;
  }
  servo_.write(currentAngle_);
}
