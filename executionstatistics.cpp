#include "executionstatistics.h"

QString formatExecutionStatistics(ExecutionStatistics es) {
  if (es.valid()) {
    return QString("%1 c, %2 μs (%3 MHz)").arg(es.cycles).arg(es.microSec(), 0, 'f', 0).arg(es.clockMHz(), 0, 'f', 2);
  }
  return "";
}
