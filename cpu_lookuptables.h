#pragma once

#include "cpu.h"

struct OpCodeEntry {
  const Instruction* operation;
  Cpu::Handler prepareOperands;
  Cpu::Handler executeInstruction;
};

using OpCodeLookUpTableType = std::array<OpCodeEntry, OpCodeTable.size()>;

extern const OpCodeLookUpTableType OpCodeLookUpTable;
