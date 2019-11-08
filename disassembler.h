#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>

class Disassembler {
public:
  static QString formatHex8(uint8_t);
  static QString formatHex16(uint16_t);

  Disassembler(const Memory&, uint16_t addr = 0);

  void setOrigin(uint16_t);
  uint16_t currentAddress() const { return address_; }
  void nextInstruction();

  QString dumpBytes(uint16_t n = 1) const;
  QString dumpWords(uint16_t n = 1) const;
  QString disassemble() const;

private:
  const Memory& memory_;
  uint16_t address_;
  uint16_t opcode_;
  Instruction instruction_;

  QString formatOperand8() const;
  QString formatOperand16() const;
};
