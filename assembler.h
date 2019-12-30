#pragma once

#include "addressrange.h"
#include "assemblyresult.h"
#include "commondefs.h"
#include "instruction.h"
#include "memory.h"
#include "operandvalue.h"
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
  void init(Address addr = DefaultOrigin);
  void initPreserveSymbols(Address = DefaultOrigin);
  void changeMode(ProcessingMode mode);
  AssemblyResult processLine(const QString&);
  AddressRange affectedAddressRange() const;
  int bytesWritten() const;

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
  ProcessingMode mode;
  int written;
  QRegularExpressionMatch match;
  uint16_t locationCounter;
  uint16_t lastLocationCounter;
  SymbolTable symbolTable;

  OperandValue resolveOperandValue(const QString&) const;
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
  void assemble(OperandsFormat mode, OperandValue = OperandValue());
  void defineSymbol(const QString&, uint16_t);
  void emitByte(uint8_t);
  void emitWord(uint16_t);
  void updateAddressRange(uint16_t);
};
