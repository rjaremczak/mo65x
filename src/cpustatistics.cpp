#include "cpustatistics.h"

QString formatExecutionStatistics(CpuStatistics es) {
  const auto megaCycles = es.cycles / 1e6;
  if (es.valid()) {
    if (es.seconds() < 0.5) {
      return QString("%1 Mc, %2 μs (%3 MHz)").arg(megaCycles).arg(es.microSec(), 0, 'f', 2).arg(es.clockMHz(), 0, 'f', 2);
    } else {
      return QString("%1 Mc, %2 s (%3 MHz)")
          .arg(megaCycles, 0, 'g', 8)
          .arg(es.seconds(), 0, 'g', 3)
          .arg(es.clockMHz(), 0, 'g', 3);
    }
  }
  return "";
}
