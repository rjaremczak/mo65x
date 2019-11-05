#pragma once

#include <cstdint>

constexpr uint8_t loByte(int word) {
  return word & 0xff;
}

constexpr uint8_t hiByte(int word) {
  return word >> 8 & 0xff;
}

constexpr uint16_t wordOf(uint8_t low, uint8_t high) {
  return static_cast<uint16_t>(low | high << 8);
}

constexpr uint16_t fromLittleEndian(const uint8_t* ptr) {
  return static_cast<uint16_t>(*ptr | *(ptr + 1) << 8);
}
