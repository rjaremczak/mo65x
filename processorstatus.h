#pragma once

#include <cstdint>

struct ProcessorStatus {
  bool negative, overflow, decimal, interrupt, zero, carry;

  void computeN(unsigned result) { negative = result & 0x80; }
  void computeZ(unsigned result) { zero = static_cast<uint8_t>(result) == 0; }
  void computeC(unsigned result) { carry = result & 0x100; }
  void computeV(unsigned op1, unsigned op2, unsigned result) { overflow = (op1 ^ result) & (op2 ^ result) & 0x80; }

  void computeNZ(unsigned result) {
    computeN(result);
    computeZ(result);
  }

  void computeNZC(unsigned result) {
    computeN(result);
    computeZ(result);
    computeC(result);
  }

  void operator=(uint8_t v) {
    this->negative = v & 0x80;
    this->overflow = v & 0x40;
    this->decimal = v & 0x08;
    this->interrupt = v & 0x04;
    this->zero = v & 0x02;
    this->carry = v & 0x01;
  }

  operator uint8_t() const {
    return static_cast<uint8_t>(negative << 7 | overflow << 6 | 0x20 | decimal << 3 | interrupt << 2 | zero << 1 | carry);
  }

  uint8_t withBreakFlag() const { return operator uint8_t() | 0x10; }
};
