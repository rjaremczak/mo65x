#pragma once

#include "addressrange.h"
#include "assemblerline.h"
#include "assemblyresult.h"
#include "commondefs.h"
#include "instruction.h"
#include "memory.h"
#include "symboltable.h"
#include <QRegularExpression>
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
  void processLine2(const QString&);
  AddressRange affectedAddressRange() const;
  size_t bytesWritten() const;

private:
  friend class AssemblerTest;
  friend class InstructionsTest;

  struct PatternEntry {
    QRegularExpression regex;
    void (Assembler::*handler)(const QRegularExpressionMatch&);
  };
  static const PatternEntry Patterns[];

  Memory& memory;
  AddressRange addressRange;
  ProcessingMode mode = ProcessingMode::EmitCode;
  size_t written = 0;
  uint16_t locationCounter = DefaultOrigin;
  uint16_t lastLocationCounter = DefaultOrigin;
  SymbolTable symbolTable;

  int operandValue(const QString&);
  int8_t operandAsBranchDisplacement(const QString&);

  void handleEmpty(const QRegularExpressionMatch&);
  void handleOrigin(const QRegularExpressionMatch&);
  void handleByte(const QRegularExpressionMatch&);
  void handleWord(const QRegularExpressionMatch&);
  void handleImpliedOrAccumulator(const QRegularExpressionMatch&);
  void handleImmediate(const QRegularExpressionMatch&);
  void handleAbsolute(const QRegularExpressionMatch&);
  void handleAbsoluteIndexedX(const QRegularExpressionMatch&);
  void handleAbsoluteIndexedY(const QRegularExpressionMatch&);
  void handleIndirect(const QRegularExpressionMatch&);
  void handleIndexedIndirectX(const QRegularExpressionMatch&);
  void handleIndirectIndexedY(const QRegularExpressionMatch&);
  void handleBranch(const QRegularExpressionMatch&);
  void assemble(const QRegularExpressionMatch&, OperandsFormat mode, int operand = 0);

  AssemblyResult defineSymbol(const QString&, uint16_t);
  void defineSymbol2(const QString&, uint16_t);
  AssemblyResult processControlCommand(const AssemblerLine&);
  AssemblyResult processInstruction(const AssemblerLine&);
  AssemblyResult cmdSetOrigin(const AssemblerLine&);
  AssemblyResult cmdEmitBytes(const AssemblerLine&);
  AssemblyResult cmdEmitWords(const AssemblerLine&);
  AssemblyResult assemble(InstructionType type, OperandsFormat mode, int operand = 0);
  void addByte(uint8_t);
  void updateAddressRange(uint16_t);
};
