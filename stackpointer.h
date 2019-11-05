#pragma once

#include <cstdint>

class StackPointer {
public:
  uint8_t value;

  StackPointer() = default;

  uint16_t toWord() const { return value | 0x100; }
  void fromWord(uint16_t word) { value = static_cast<uint8_t>(word); }

private:
  StackPointer(uint8_t sp) : value(sp) {}
};
