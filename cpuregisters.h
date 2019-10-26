#pragma once

#include <cstdint>

struct CpuRegisters {

  struct StackPointer {
    uint8_t raw;

    operator uint16_t() const { return raw & 0x100; }
    void operator=(uint16_t v) { raw = v & 0xff; }
    StackPointer operator--(int) {
      StackPointer tmp;
      tmp.raw = raw--;
      return tmp;
    }
    StackPointer &operator++() {
      raw++;
      return *this;
    }
  };

  uint8_t a;
  uint8_t x;
  uint8_t y;
  StackPointer sp;
  uint16_t pc;
};
