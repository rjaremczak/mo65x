#pragma once

#include <cstdint>

struct StackPointer {
  const uint16_t base = 0x100;
  uint8_t offset;

  uint16_t address() const { return base | offset; }
  void decrement() { --offset; }
  void increment() { ++offset; }
};
