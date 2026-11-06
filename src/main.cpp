#include <Arduino.h>

#include "config.h"
#include "emg_filter.h"
#include "servo_handler.h"
#include "signal_logic.h"

namespace {
EmgFilter emgFilter;
ServoHandler servoHandler;

enum class CalibrationMode { kIdle, kRestBaseline, kContractionPeak };
CalibrationMode calibrationMode = CalibrationMode::kIdle;

bool baselineCalibrated = false;
bool peakCalibrated = false;
bool manualMode = false;
bool autoModeEnabled = false;
bool contractionActive = false;
float baselineAdc = 0.0f;
float peakAmplitude = 0.0f;
float thresholdAmplitude = 0.0f;
float latestEnvelope = 0.0f;
float latestMav = 0.0f;
float latestRms = 0.0f;
float restAccumulator = 0.0f;
uint32_t restSampleCount = 0;
uint32_t calibrationStartedAt = 0;
uint32_t lastSampleAt = 0;
uint32_t lastTelemetryAt = 0;

void printHelp() {
  Serial.println(F("Perintah: r=kalibrasi rileks, c=kalibrasi kontraksi, a=auto, o=buka, m=genggam, ?=bantuan"));
}

void startRestCalibration() {
  calibrationMode = CalibrationMode::kRestBaseline;
  baselineCalibrated = false;
  peakCalibrated = false;
  manualMode = false;
  autoModeEnabled = false;
  contractionActive = false;
  restAccumulator = 0.0f;
  restSampleCount = 0;
  peakAmplitude = 0.0f;
  emgFilter.reset();
  servoHandler.setTarget(config::kServoOpenDeg);
  calibrationStartedAt = millis();
  Serial.println(F("Kalibrasi baseline 3 detik: rilekskan otot dan diam."));
}

void startPeakCalibration() {
  if (!baselineCalibrated) {
    Serial.println(F("Baseline belum siap. Kirim 'r' saat otot rileks terlebih dahulu."));
    return;
  }
  calibrationMode = CalibrationMode::kContractionPeak;
  peakCalibrated = false;
  manualMode = false;
  autoModeEnabled = false;
  contractionActive = false;
  peakAmplitude = 0.0f;
  emgFilter.reset();
  servoHandler.setTarget(config::kServoOpenDeg);
  calibrationStartedAt = millis();
  Serial.println(F("Kalibrasi kontraksi 3 detik: kontraksikan otot kuat dan stabil."));
}

void handleSerial() {
  while (Serial.available() > 0) {
    const char command = static_cast<char>(Serial.read());
    switch (command) {
      case 'r':
      case 'R':
        startRestCalibration();
        break;
      case 'c':
      case 'C':
        startPeakCalibration();
        break;
      case 'a':
      case 'A':
        if (baselineCalibrated && peakCalibrated) {
          calibrationMode = CalibrationMode::kIdle;
          manualMode = false;
          autoModeEnabled = true;
          contractionActive = false;
          emgFilter.reset();
          latestEnvelope = 0.0f;
          Serial.println(F("Kontrol otomatis aktif."));
        } else {
          Serial.println(F("Kalibrasi baseline ('r') dan kontraksi ('c') harus berhasil sebelum mode otomatis."));
        }
        break;
      case 'o':
      case 'O':
        calibrationMode = CalibrationMode::kIdle;
        manualMode = true;
        autoModeEnabled = false;
        contractionActive = false;
        servoHandler.setTarget(config::kServoOpenDeg);
        Serial.println(F("Mode manual: buka."));
        break;
      case 'm':
      case 'M':
        calibrationMode = CalibrationMode::kIdle;
        manualMode = true;
        autoModeEnabled = false;
        contractionActive = false;
        servoHandler.setTarget(config::kServoGripDeg);
        Serial.println(F("Mode manual: sudut genggam uji."));
        break;
      case '?':
        printHelp();
        break;
      default:
        // Ignore line endings and unknown characters.
        break;
    }
  }
}

void processSample(int rawAdc, uint32_t now) {
  if (calibrationMode == CalibrationMode::kRestBaseline) {
    restAccumulator += static_cast<float>(rawAdc);
    ++restSampleCount;
    if (now - calibrationStartedAt >= config::kCalibrationDurationMs) {
      calibrationMode = CalibrationMode::kIdle;
      if (restSampleCount == 0) {
        Serial.println(F("Kalibrasi baseline gagal: tidak ada sampel."));
        return;
      }
      baselineAdc = restAccumulator / static_cast<float>(restSampleCount);
      emgFilter.setBaseline(baselineAdc);
      baselineCalibrated = true;
      Serial.print(F("Baseline tersimpan: "));
      Serial.println(baselineAdc, 1);
      Serial.println(F("Berikutnya kirim 'c' dan kontraksikan otot."));
    }
    return;
  }

  if (!baselineCalibrated) return;
  emgFilter.update(rawAdc);
  latestMav = emgFilter.mav();
  latestRms = emgFilter.rms();
  latestEnvelope = config::kUseRmsFeature ? latestRms : latestMav;

  if (calibrationMode == CalibrationMode::kContractionPeak) {
    if (latestEnvelope > peakAmplitude) peakAmplitude = latestEnvelope;
    if (now - calibrationStartedAt >= config::kCalibrationDurationMs) {
      calibrationMode = CalibrationMode::kIdle;
      if (peakAmplitude < config::kPeakAcceptanceFloorAdc) {
        peakCalibrated = false;
        Serial.println(F("Kalibrasi kontraksi terlalu kecil/tidak terbaca. Periksa sensor dan ulangi 'c'."));
        return;
      }
      thresholdAmplitude = peakAmplitude * config::kThresholdFractionOfPeak;
      if (thresholdAmplitude < config::kThresholdFloorAdc) {
        thresholdAmplitude = config::kThresholdFloorAdc;
      }
      if (thresholdAmplitude >= peakAmplitude) {
        peakCalibrated = false;
        Serial.println(F("Rentang sinyal terlalu kecil untuk threshold. Periksa sensor dan kalibrasi ulang."));
        return;
      }
      peakCalibrated = true;
      Serial.print(F("Puncak kontraksi: "));
      Serial.print(peakAmplitude, 1);
      Serial.print(F(" ADC; threshold: "));
      Serial.println(thresholdAmplitude, 1);
      Serial.println(F("Kirim 'a' untuk mengaktifkan kontrol otomatis."));
    }
    return;
  }

  if (autoModeEnabled && !manualMode && peakCalibrated) {
    const float releaseThreshold = thresholdAmplitude * config::kReleaseFractionOfThreshold;
    contractionActive = updateContractionState(
        latestEnvelope, thresholdAmplitude, releaseThreshold, contractionActive);
    const int target = contractionActive
                           ? mapSignalToServo(latestEnvelope, thresholdAmplitude,
                                              peakAmplitude, config::kServoOpenDeg,
                                              config::kServoGripDeg)
                           : config::kServoOpenDeg;
    servoHandler.setTarget(target);
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(config::kEmgPin, INPUT);
  analogReadResolution(12);
  servoHandler.begin(config::kServoPin, config::kServoOpenDeg);
  Serial.println();
  Serial.println(F("EMG prosthetic-hand prototype firmware; bench test only."));
  Serial.print(F("Fitur kontrol: "));
  Serial.println(config::kUseRmsFeature ? F("RMS") : F("MAV"));
  printHelp();
  Serial.println(F("Pastikan output sensor aman untuk ADC ESP32 dan servo punya catu daya terpisah."));
}

void loop() {
  handleSerial();
  const uint32_t now = millis();

  if (now - lastSampleAt >= config::kSampleIntervalMs) {
    lastSampleAt = now;
    const int rawAdc = analogRead(config::kEmgPin);
    processSample(rawAdc, now);

    if (now - lastTelemetryAt >= config::kTelemetryIntervalMs) {
      lastTelemetryAt = now;
      Serial.print(F("adc="));
      Serial.print(rawAdc);
      Serial.print(F(" env="));
      Serial.print(latestEnvelope, 1);
      Serial.print(F(" mav="));
      Serial.print(latestMav, 1);
      Serial.print(F(" rms="));
      Serial.print(latestRms, 1);
      Serial.print(F(" feature="));
      Serial.print(latestEnvelope, 1);
      Serial.print(F(" mode="));
      Serial.print(config::kUseRmsFeature ? F("RMS") : F("MAV"));
      Serial.print(F(" threshold="));
      Serial.print(thresholdAmplitude, 1);
      Serial.print(F(" active="));
      Serial.print(contractionActive ? 1 : 0);
      Serial.print(F(" target="));
      Serial.print(servoHandler.targetAngle());
      Serial.print(F(" current="));
      Serial.println(servoHandler.currentAngle());
    }
  }

  servoHandler.update();
}
