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
#include <regex>
#include <vector>

class Assembler
{
public:
  enum class ProcessingMode { ScanForSymbols, EmitCode };

  static constexpr uint16_t DefaultOrigin = 0;

  const auto& symbols() const { return m_symbols; }

  Assembler(Memory&);
  void init(Address addr = DefaultOrigin);
  void initPreserveSymbols(Address = DefaultOrigin);
  void changeMode(ProcessingMode m_mode);
  AssemblyResult processLine(const QString&);
  AddressRange affectedAddressRange() const;
  int bytesWritten() const;

private:
  friend class AssemblerTest;
  friend class InstructionsTest;

  struct PatternEntry {
    using Handler = void (Assembler::*)();
    const std::regex regex;
    const Handler handler;

    PatternEntry(const QString& pattern, const Handler handler);
  };
  static const PatternEntry Patterns[];

  Memory& m_memory;
  AddressRange m_addressRange;
  ProcessingMode m_mode;
  int m_written;
  uint16_t m_locationCounter;
  uint16_t m_lastLocationCounter;
  SymbolTable m_symbols;
  QString m_operation;
  QString m_operand;

  OperandValue operandValue(const QString&) const;
  OperandValue operandValue() const;
  int8_t operandAsBranchDisplacement() const;

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
  void assemble(OperandsFormat m_mode, OperandValue = OperandValue());
  void defineSymbol(QString, uint16_t);
  void emitByte(uint8_t);
  void emitWord(uint16_t);
  void updateAddressRange(uint16_t);
};
