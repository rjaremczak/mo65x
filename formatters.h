#pragma once

#include "executionstatistics.h"
#include <QString>
#include <cstdint>

inline QString formatHexByte(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

inline QString formatHexWord(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}

inline QString formatExecutionStatistics(const ExecutionStatistics es) {
  if (es.valid()) {
    return QString("%1 c, %2 μs (%3 MHz)").arg(es.cycles).arg(es.microSec(), 0, 'f', 0).arg(es.clockMHz(), 0, 'f', 2);
  }
  return "";
}
