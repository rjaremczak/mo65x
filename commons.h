#pragma once

#include <QString>
#include <algorithm>
#include <stdint.h>
#include <vector>

using Data = std::vector<uint8_t>;

struct AddressRange {
  uint16_t first;
  uint16_t last;

  bool overlapsWith(AddressRange range) const {
    return first == std::clamp(first, range.first, range.last) || last == std::clamp(last, range.first, range.last);
  }
};

inline QString formatHexByte(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

inline QString formatHexWord(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}

using FileOperationCallBack = std::function<void(qint64)>;
