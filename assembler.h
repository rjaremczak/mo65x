#pragma once

#include "addressrange.h"
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
  AssemblyResult processLine2(const QString&);
  AddressRange affectedAddressRange() const;
  size_t bytesWritten() const;

private:
  friend class AssemblerTest;
  friend class InstructionsTest;

  struct PatternEntry {
    using Handler = void (Assembler::*)();
    const QRegularExpression regex;
    const Handler handler;

    PatternEntry(const QString& pattern, const Handler handler);
  };
  static const PatternEntry Patterns[];

  Memory& memory;
  AddressRange addressRange;
  ProcessingMode mode = ProcessingMode::EmitCode;
  size_t written = 0;
  QRegularExpressionMatch match;
  uint16_t locationCounter = DefaultOrigin;
  uint16_t lastLocationCounter = DefaultOrigin;
  SymbolTable symbolTable;

  int resolveAsInt(const QString&) const;
  int8_t operandAsBranchDisplacement() const;
  QString operation() const;
  QString operand() const;

  void handleNoOperation();
  void handleSetLocationCounter();
  void handleEmitBytes();
  void handleEmitWords();
  void handleImpliedOrAccumulator();
  void handleImmediate();
  void handleAbsolute();
  void handleAbsoluteIndexedX();
  void handleAbsoluteIndexedY();
  void handleIndirect();
  void handleIndexedIndirectX();
  void handleIndirectIndexedY();
  void handleBranch();
  void assemble(OperandsFormat mode, int operand = 0);

  void defineSymbol2(const QString&, uint16_t);
  // AssemblyResult assemble(InstructionType type, OperandsFormat mode, int operand = 0);
  void emitByte(uint8_t);
  void updateAddressRange(uint16_t);
};
