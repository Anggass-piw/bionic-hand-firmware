#include <cassert>
#include <cmath>
#include <iostream>

#include "emg_filter.h"
#include "signal_logic.h"

int main() {
  EmgFilter filter;
  filter.setBaseline(2000.0f);

  assert(std::fabs(filter.update(2000) - 0.0f) < 0.001f);
  assert(std::fabs(filter.update(2100) - 50.0f) < 0.001f);
  assert(std::fabs(filter.mav() - 50.0f) < 0.001f);
  assert(std::fabs(filter.rms() - std::sqrt(5000.0f)) < 0.01f);

  // With a 25-sample window, one 100-count excursion averages to 4 counts
  // after 25 baseline samples have fully entered the window.
  filter.reset();
  filter.setBaseline(1000.0f);
  assert(std::fabs(filter.update(1100) - 100.0f) < 0.001f);
  for (int i = 0; i < 24; ++i) filter.update(1000);
  assert(std::fabs(filter.mav() - 4.0f) < 0.001f);
  assert(std::fabs(filter.rms() - 20.0f) < 0.001f);
  assert(std::fabs(filter.update(1000) - 0.0f) < 0.001f);

  const int open = 10;
  const int grip = 95;
  assert(mapSignalToServo(10.0f, 10.0f, 100.0f, open, grip) == open);
  assert(mapSignalToServo(55.0f, 10.0f, 100.0f, open, grip) >= 52);
  assert(mapSignalToServo(100.0f, 10.0f, 100.0f, open, grip) == grip);
  assert(mapSignalToServo(500.0f, 10.0f, 100.0f, open, grip) == grip);
  assert(mapSignalToServo(50.0f, 50.0f, 50.0f, open, grip) == open);

  assert(!updateContractionState(11.0f, 12.0f, 8.0f, false));
  assert(updateContractionState(12.0f, 12.0f, 8.0f, false));
  assert(updateContractionState(9.0f, 12.0f, 8.0f, true));
  assert(!updateContractionState(8.0f, 12.0f, 8.0f, true));

  std::cout << "All native logic tests passed.\n";
  return 0;
}
