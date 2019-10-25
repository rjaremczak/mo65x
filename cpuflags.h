#pragma once

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
};
