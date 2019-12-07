#pragma once

#include "addressrange.h"
#include "assemblerline.h"
#include "assemblyresult.h"
#include "commondefs.h"
#include "instruction.h"
#include "memory.h"
#include "symboltable.h"
#include <QString>
#include <iterator>
#include <map>
#include <optional>
#include <vector>

class Assembler
{
public:
  enum class ProcessingMode { ScanForSymbols, EmitCode };

  static constexpr uint16_t DefaultOrigin = 0;

  const auto& symbols() const { return symbolTable; }

  Assembler(Memory&);
  void init(uint16_t addr = DefaultOrigin);
  void clearSymbols();
  void changeMode(ProcessingMode mode);
  AssemblyResult processLine(const QString&);
  std::optional<int> symbol(const QString&) const;
  AddressRange affectedAddressRange() const;
  size_t bytesWritten() const;

private:
  friend class AssemblerTest;
  friend class InstructionsTest;

  Memory& memory;
  AddressRange addressRange;
  ProcessingMode mode = ProcessingMode::EmitCode;
  size_t written = 0;
  uint16_t locationCounter = DefaultOrigin;
  uint16_t lastLocationCounter = DefaultOrigin;
  SymbolTable symbolTable;

  AssemblyResult defineSymbol(const QString&, uint16_t);
  AssemblyResult processControlCommand(const AssemblerLine&);
  AssemblyResult processInstruction(const AssemblerLine&);
  AssemblyResult cmdSetOrigin(const AssemblerLine&);
  AssemblyResult cmdEmitBytes(const AssemblerLine&);
  AssemblyResult cmdEmitWords(const AssemblerLine&);
  AssemblyResult assemble(InstructionType type, OperandsFormat mode, int operand = 0);
  void addByte(uint8_t);
  void updateAddressRange(uint16_t);
};
