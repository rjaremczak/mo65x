#pragma once

#include "cpudefs.h"

struct StackPointer {
  uint8_t offset;

  uint16_t address() const { return StackPointerBase | offset; }
  void decrement() { --offset; }
  void increment() { ++offset; }
};
