#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>

class Disassembler {
  const Memory& memory;
  uint16_t address;
  uint16_t opcode;
  Instruction instruction;

  QString formatOperand8() const;
  QString formatOperand16() const;

public:
  Disassembler(const Memory&, uint16_t addr = 0);

  void setAddress(uint16_t);
  uint16_t currentAddress() const { return address; }
  void nextInstruction();

  QString dumpBytes(uint16_t n = 1) const;
  QString dumpWords(uint16_t n = 1) const;
  QString disassemble() const;
};
