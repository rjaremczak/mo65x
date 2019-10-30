#pragma once

#include "instruction.h"
#include "memory.h"

class Assembler
{
public:
  Assembler(Memory& memory, uint16_t origin);
  auto address() const { return address_; }
  void ins(InstructionType, AddressingMode = Implied, uint16_t operand = 0);

private:
  Memory& memory_;
  uint16_t address_;
};
