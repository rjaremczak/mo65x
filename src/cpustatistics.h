#pragma once

#include "commondefs.h"
#include <QString>
#include <cstdint>

struct CpuStatistics {
  long cycles = 0;
  std::chrono::nanoseconds duration = std::chrono::nanoseconds::zero();

  void reset() { *this = CpuStatistics(); }
  bool valid() const { return cycles > 0 && duration != std::chrono::nanoseconds::zero(); }
  double microSec() const { return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count(); }
  double seconds() const { return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count(); }
  double clockMHz() const { return cycles / microSec(); }

  CpuStatistics operator-(const CpuStatistics& e) const { return {cycles - e.cycles, duration - e.duration}; }
};

QString formatExecutionStatistics(CpuStatistics es);
