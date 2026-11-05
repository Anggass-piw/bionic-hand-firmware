#pragma once

int mapSignalToServo(float signal, float threshold, float peak,
                     int openAngle, int gripAngle);
bool updateContractionState(float signal, float onsetThreshold,
                            float releaseThreshold, bool wasActive);
