#pragma once

#include "processorstatus.h"
#include "stackpointer.h"
#include <cstdint>

struct Registers {
  uint8_t a;
  uint8_t x;
  uint8_t y;
  Address pc;
  StackPointer sp;
  ProcessorStatus p;
};
