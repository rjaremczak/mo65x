#include "cpustatistics.h"

QString formatExecutionStatistics(CpuStatistics es) {
  if (es.valid()) {
    if (es.seconds() < 0.5) {
      return QString("%1 cycles, %2 μs (%3 MHz)").arg(es.cycles).arg(es.microSec(), 0, 'f', 2).arg(es.clockMHz(), 0, 'f', 2);
    } else {
      return QString("%1 cycles, %2 s (%3 MHz)")
          .arg(static_cast<double>(es.cycles), 0, 'g', 3)
          .arg(es.seconds(), 0, 'g', 3)
          .arg(es.clockMHz(), 0, 'g', 3);
    }
  }
  return "";
}
