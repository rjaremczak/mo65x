#pragma once

#include <QString>
#include <algorithm>
#include <stdint.h>
#include <vector>

using Data = std::vector<uint8_t>;

inline QString formatHexByte(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

inline QString formatHexWord(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}

using FileOperationCallBack = std::function<void(qint64)>;
