#pragma once

#include <cstdint>

struct CpuRegisters {

  struct StackPointer {
    uint8_t value;

    operator uint16_t() const { return value & 0x100; }
    void operator=(uint16_t v) { value = v & 0xff; }
  };

  uint8_t a;
  uint8_t x;
  uint8_t y;
  StackPointer sp;
  uint16_t pc;
};
