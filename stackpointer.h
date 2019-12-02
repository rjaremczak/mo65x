#pragma once

#include "cpudefs.h"

struct StackPointer {
  uint8_t offset;

  Address address() const { return StackPointerBase | offset; }
};
