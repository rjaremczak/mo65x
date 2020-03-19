#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>

class Disassembler {
public:
  Disassembler(const Memory&, Address addr = 0);
  void setOrigin(Address);
  Address address() const { return m_address; }
  void nextInstruction();
  QString disassemble() const;

private:
  const Memory& m_memory;
  Address m_address;
  uint8_t m_opcode;
  Instruction m_instruction;

  QString formatOperand8() const;
  QString formatOperand16() const;
};
