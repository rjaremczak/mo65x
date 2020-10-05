#pragma once

#include "addressrange.h"
#include "assemblyresult.h"
#include "commondefs.h"
#include "instruction.h"
#include "memorychip.h"
#include "operandvalue.h"
#include "symboltable.h"
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

  Assembler(MemoryChip&, SymbolTable&);

  int bytesWritten() const;
  AddressRange affectedAddressRange() const;

  void init(Address addr = DefaultOrigin);
  void initPreserveSymbols(Address = DefaultOrigin);
  void changeMode(ProcessingMode m_mode);
  AssemblyResult processLine(const std::string&);

private:
  friend class InstructionsTest;

  struct PatternEntry {
    using Handler = void (Assembler::*)();
    const std::regex regex;
    const Handler handler;

    PatternEntry(const std::string& pattern, const Handler handler);
  };
  static const PatternEntry Patterns[];

  MemoryChip& m_memory;
  SymbolTable& m_symbols;

  AddressRange m_addressRange;
  ProcessingMode m_mode;
  int m_written;
  Address m_locationCounter;
  Address m_lastLocationCounter;
  std::string m_operation;
  std::string m_operand;

  OperandValue operandValue(const std::string&) const;
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
  void defineSymbol(const std::string_view, uint16_t);
  void assemble(OperandsFormat m_mode, OperandValue = OperandValue());
  void emitByte(uint8_t);
  void emitWord(uint16_t);
  void updateAddressRange(uint16_t);
};
