#pragma once

#include <cstdint>

constexpr uint16_t little_endian_word(const uint8_t* ptr) {
  return static_cast<uint16_t>(*ptr | *(ptr + 1) << 8);
}
