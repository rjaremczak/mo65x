#pragma once

#include "instruction.h"
#include "memorychip.h"

class Disassembler {
public:
  Disassembler(const MemoryChip&, Address addr = 0);
  void setOrigin(Address);
  Address address() const { return m_address; }
  void nextInstruction();
  std::string disassemble() const;

private:
  const MemoryChip& m_memory;
  Address m_address;
  uint8_t m_opcode;
  Instruction m_instruction;

  std::string formatOperand8() const;
  std::string formatOperand16() const;
};
