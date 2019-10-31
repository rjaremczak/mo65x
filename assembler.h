#pragma once

#include "instruction.h"
#include "memory.h"
#include <QString>
#include <set>

class Assembler
{
public:
  Assembler(Memory& memory, uint16_t origin = 0);
  void setOrigin(uint16_t origin) { address_ = origin; }
  auto currentAddress() const { return address_; }
  bool enter(InstructionType type, AddressingMode mode = NoOperands, uint16_t operand = 0);
  bool enter(QString);

private:
  Memory& memory_;
  uint16_t address_;
};
