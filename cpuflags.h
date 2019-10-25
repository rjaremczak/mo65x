#pragma once

#include <cstdint>
#include <bitset>

struct CpuFlags {
  bool n;
  bool v;
  bool b;
  bool d;
  bool i;
  bool z;
  bool c;

  void computeNZ(unsigned result) {
    n = result & 0x80;
    z = !result;
  }

  void computeC(unsigned result) { c = result & 0x100; }

  void computeNZC(unsigned result) {
    computeNZ(result);
    computeC(result);
  }

  void computeV(unsigned op1, unsigned op2, unsigned result) {
    v = (op1 ^ result) & (op2 ^ result) & 0x80;
  }

  uint8_t toByte() {
    return static_cast<uint8_t>(n << 7 | v << 6 | 0x20 | b << 4 | d << 3 | i << 2 | z << 1 | c);
  }

  void fromByte(uint8_t v) {
    n = v & 0x80;
    v = v & 0x40;
    b = v & 0x10;
    d = v & 0x08;
    i = v & 0x04;
    z = v & 0x02;
    c = v & 0x01;
  }
};
