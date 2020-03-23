#pragma once

#include "cpustatistics.h"
#include <QString>
#include <cstdint>
#include <fmt/format.h>

inline auto formatHex8(uint8_t val) {
  return fmt::format("{:02X}",val);
}

inline auto formatHex16(uint16_t val) {
  return fmt::format("{:04X}",val);
}

inline auto formatOperand8(uint8_t val) {
  return fmt::format("${:02X}",val);
}

inline auto formatOperand16(uint16_t val) {
  return fmt::format("${:04X}",val);
}
