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
  AddressRange affectedAddressRange() const;
  size_t bytesWritten() const;

private:
  friend class AssemblerTest;
  friend class InstructionsTest;

  using PatternHandler = void (Assembler::*)(const QString&);
  static const std::pair<QRegularExpression, PatternHandler> patternHandlers[];

  Memory& memory;
  AddressRange addressRange;
  ProcessingMode mode = ProcessingMode::EmitCode;
  size_t written = 0;
  uint16_t locationCounter = DefaultOrigin;
  uint16_t lastLocationCounter = DefaultOrigin;
  SymbolTable symbolTable;

  void handleEmpty(const QString&);
  void handleOrigin(const QString&);
  void handleByte(const QString&);
  void handleWord(const QString&);
  void handleNoOperands(const QString&);
  void handleImmediate(const QString&);
  void handleAbsolute(const QString&);
  void handleAbsoluteIndexedX(const QString&);
  void handleAbsoluteIndexedY(const QString&);
  void handleIndirect(const QString&);
  void handleIndexedIndirectX(const QString&);
  void handleIndirectIndexedY(const QString&);
  void handleBranch(const QString&);

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
