#pragma once

#include <cstdint>

struct ProcessorStatus {
  bool n, v, d, i, z, c;

  void computeN(unsigned result) { n = result & 0x80; }
  void computeZ(unsigned result) { z = static_cast<uint8_t>(result) == 0; }
  void computeC(unsigned result) { c = result & 0x100; }
  void computeV(unsigned op1, unsigned op2, unsigned result) { v = (op1 ^ result) & (op2 ^ result) & 0x80; }

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
    this->n = v & 0x80;
    this->v = v & 0x40;
    this->d = v & 0x08;
    this->i = v & 0x04;
    this->z = v & 0x02;
    this->c = v & 0x01;
  }

  operator uint8_t() const { return static_cast<uint8_t>(n << 7 | v << 6 | 0x20 | d << 3 | i << 2 | z << 1 | c); }

  uint8_t withBreakFlag() const { return operator uint8_t() | 0x10; }
};
