#pragma once

#include <cstdint>

struct ProcessorStatus {
  static constexpr uint8_t NegativeBitMask = 0x80;
  static constexpr uint8_t OverflowBitMask = 0x40;
  static constexpr uint8_t BreakBitMask = 0x10;
  static constexpr uint8_t DecimalBitMask = 0x08;
  static constexpr uint8_t InterruptBitMask = 0x04;
  static constexpr uint8_t ZeroBitMask = 0x02;
  static constexpr uint8_t CarryBitMask = 0x01;

  bool negative;
  bool overflow;
  bool decimal;
  bool interrupt;
  bool zero;
  bool carry;

  void computeN(uint16_t result) { negative = result & 0x80; }
  void computeZ(uint16_t result) { zero = !(result & 0xff); }
  void computeC(uint16_t result) { carry = result & 0xff00; }
  void computeV(uint16_t op1, uint16_t op2, uint16_t result) { overflow = (op1 ^ result) & (op2 ^ result) & 0x80; }

  void computeNZ(uint16_t result) {
    computeN(result);
    computeZ(result);
  }

  void computeNZC(uint16_t result) {
    computeN(result);
    computeZ(result);
    computeC(result);
  }

  void operator=(uint8_t v) {
    this->negative = v & NegativeBitMask;
    this->overflow = v & OverflowBitMask;
    this->decimal = v & DecimalBitMask;
    this->interrupt = v & InterruptBitMask;
    this->zero = v & ZeroBitMask;
    this->carry = v & CarryBitMask;
  }

  operator uint8_t() const {
    return (negative ? NegativeBitMask : 0) | (overflow ? OverflowBitMask : 0) | (decimal ? DecimalBitMask : 0) |
           (interrupt ? InterruptBitMask : 0) | (zero ? ZeroBitMask : 0) | (carry ? CarryBitMask : 0);
  }

  void fromByte(uint8_t b) { *this = b; }
  uint8_t toByte() const { return *this; }
};
