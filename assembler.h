#pragma once

#include "instruction.h"
#include "memory.h"

class Assembler
{
public:
  Assembler(Memory& memory, uint16_t origin = 0);
  void setOrigin(uint16_t origin) { address_ = origin; }
  auto currentAddress() const { return address_; }
  void enter(InstructionType, AddressingMode = Implied, uint16_t operand = 0);
  void enter(const char*);
  bool valid() const { return valid_; }

private:
  Memory& memory_;
  uint16_t address_;
  bool valid_;

  const Instruction* find(InstructionType, AddressingMode) const;
};
