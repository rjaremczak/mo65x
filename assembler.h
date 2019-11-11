#pragma once

#include "instruction.h"
#include <QString>
#include <iterator>
#include <vector>

class Assembler
{
public:
  using Buffer = std::vector<uint8_t>;

  Assembler();
  void reset();
  void setOrigin(uint16_t);
  bool assemble(InstructionType type, AddressingMode mode = NoOperands, int operand = 0);
  bool assemble(const char*);
  const Buffer& data() const { return buffer; }
  uint16_t address() { return static_cast<uint16_t>(origin + buffer.size()); }

private:
  Buffer buffer;
  std::back_insert_iterator<Buffer> iterator;
  uint16_t origin;
};
