#pragma once

#include <cstdint>

constexpr uint8_t loByte(unsigned word) {
  return word & 0xff;
}

constexpr uint8_t hiByte(unsigned word) {
  return word >> 8 & 0xff;
}

constexpr uint16_t wordOf(uint8_t low, uint8_t high) {
  return static_cast<uint16_t>(low | high << 8);
}
