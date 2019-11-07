#pragma once

#include <cstdint>

struct StackPointer {
  const uint16_t base = 0x100;
  uint8_t offset;

  void decrement() { --offset; }
  void increment() { ++offset; }

  operator uint16_t() const { return base | offset; }
};
