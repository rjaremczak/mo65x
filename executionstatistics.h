#pragma once

#include "defs.h"
#include <cstdint>

struct ExecutionStatistics {
  long cycles = 0;
  Duration duration = Duration::zero();

  bool valid() const { return cycles > 0 && duration != Duration::zero(); }
  double microSec() const { return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count(); }
  double clockMHz() const { return cycles / microSec(); }
};
