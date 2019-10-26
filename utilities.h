#pragma once

#include <cstdint>

constexpr uint8_t loByte(uint16_t word) {
  return word & 0xff;
}

constexpr uint8_t hiByte(uint16_t word) {
  return word >> 8 & 0xff;
}

constexpr uint16_t wordOf(uint8_t low, uint16_t high) {
  return static_cast<uint16_t>(low | high << 8);
}
