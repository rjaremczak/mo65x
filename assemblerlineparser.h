#pragma once

#include "assemblyresult.h"
#include "commondefs.h"
#include "controlcommand.h"
#include "instruction.h"
#include "operandsformat.h"
#include "symboltable.h"
#include <QString>
#include <vector>

struct AssemblerLineModel {
  QString label;
  std::vector<int> operands;
  ControlCommand controlCommand = ControlCommand::None;
  InstructionType instructionType = InstructionType::KIL;
  OperandsFormat addressingMode = OperandsFormat::Implied;

  uint8_t instructionSize() const;

  AssemblerLineModel create(const QString& line, const SymbolTable& symbols, Address pc);
};
