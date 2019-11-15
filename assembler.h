#pragma once

#include "assemblerline.h"
#include "commons.h"
#include "instruction.h"
#include <QMetaType>
#include <QString>
#include <iterator>
#include <map>
#include <optional>
#include <vector>

class Assembler
{
  Q_GADGET

public:
  using Symbols = std::map<QString, uint16_t>;

  static constexpr uint16_t DefaultOrigin = 0;

  enum class Pass { ScanForSymbols, Assembly };
  enum class Result {
    Ok,
    OriginAlreadyDefined,
    SymbolAlreadyDefined,
    UndefinedSymbol,
    MissingOperand,
    NumericOperandRequired,
    SyntaxError,
    CommandProcessingError,
    FatalProcessingError
  };

  Q_ENUM(Result)

  Assembler();

  uint16_t origin() const { return origin_; }
  const auto& symbols() const { return symbols_; }
  auto locationCounter() const { return locationCounter_; }
  const Data& code() const { return code_; }

  void reset(Pass = Pass::Assembly);
  Result setOrigin(uint16_t addr);
  uint8_t lastInstructionByte(size_t) const;
  Result process(const QString&);
  std::optional<int> symbol(const QString&) const;

private:
  Pass pass_ = Pass::Assembly;
  bool originDefined_ = false;
  uint16_t origin_ = DefaultOrigin;
  uint16_t locationCounter_ = DefaultOrigin;
  Data code_;
  size_t lastInstructionIndex_;
  std::back_insert_iterator<Data> iterator_;
  Symbols symbols_;

  Result addSymbol(const QString&, uint16_t);
  Result processCommand(const AssemblerLine& line);
  Result processInstruction(const AssemblerLine& line);
  Result assemble(InstructionType type, OperandsFormat mode, int operand = 0);
  void addByte(uint8_t);
};
