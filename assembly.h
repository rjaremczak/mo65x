#pragma once

#include "assemblyresult.h"
#include "commondefs.h"
#include "instruction.h"
#include "operandsformat.h"
#include "symboltable.h"
#include <QString>
#include <vector>

struct Assembly {
  enum class Command { None, Origin, Byte, Word, Instruction };

  QString label;
  std::vector<int> operands;
  Command command = Command::None;
  const Instruction* instruction = nullptr;

  uint8_t opCode() const;

  static Assembly process(const QString& line, const SymbolTable& symbols, Address pc);
};
