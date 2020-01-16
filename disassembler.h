#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>

class Disassembler {
public:
  Disassembler(const Memory&, Address addr = 0);

  void setOrigin(Address);
  Address currentAddress() const { return address; }
  void nextInstruction();

  QString dumpBytes(uint16_t n = 1) const;
  QString dumpWords(uint16_t n = 1) const;
  QString disassemble() const;

private:
  const Memory& memory;
  Address address;
  uint8_t opcode;
  Instruction instruction;

  QString formatOperand8() const;
  QString formatOperand16() const;
};
