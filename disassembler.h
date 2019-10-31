#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>

class Disassembler {
public:
  static QString formatHex8(uint8_t);
  static QString formatHex16(uint16_t);
  static QString formatNoOperands(const QString& mnemonic);

  Disassembler(const Memory&, uint16_t addr = 0);

  void setAddress(uint16_t);
  uint16_t currentAddress() const { return address; }
  void nextInstruction();

  QString dumpBytes(uint16_t n = 1) const;
  QString dumpWords(uint16_t n = 1) const;
  QString disassemble() const;

private:
  const Memory& memory;
  uint16_t address;
  uint16_t opcode;
  Instruction instruction;

  QString formatOperand8() const;
  QString formatOperand16() const;
};
