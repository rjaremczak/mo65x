#pragma once

#include "cpustatistics.h"
#include <QString>
#include <cstdint>

inline QString formatHexByte(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

inline QString formatHexWord(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}
