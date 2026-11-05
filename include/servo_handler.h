#pragma once

#include <ESP32Servo.h>

class ServoHandler {
 public:
  ServoHandler();
  void begin(int pin, int initialAngle);
  void setTarget(int angle);
  void update();
  int currentAngle() const { return currentAngle_; }
  int targetAngle() const { return targetAngle_; }

 private:
  Servo servo_;
  int currentAngle_;
  int targetAngle_;
  uint32_t lastUpdateMs_;
};
