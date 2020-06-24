#pragma once

#include <cstdint>

struct OperandPtr {
  uint8_t* lo;
  uint8_t* hi;

  uint16_t word() const { return static_cast<uint16_t>(*lo | *hi << 8); }
};
