#pragma once

#include <cstdint>

class StackPointer {
public:
  StackPointer() = default;

  operator uint16_t() const { return value & 0x100; }

  void operator=(uint16_t v) { value = v & 0xff; }

  StackPointer operator--(int) { return StackPointer(value--); }

  StackPointer& operator++() {
    ++value;
    return *this;
  }

  uint8_t toByte() const { return value; }

  void fromByte(uint8_t b) { value = b; }

private:
  uint8_t value;

  StackPointer(uint8_t sp) : value(sp) {}
};
