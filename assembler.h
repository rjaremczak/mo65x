#pragma once

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
  using Buffer = std::vector<uint8_t>;
  using Symbols = std::map<QString, uint16_t>;

  static constexpr uint16_t DefaultOrigin = 0;

  enum class Pass { Symbols, Assembly };
  enum class Result { Ok, OriginAlreadyDefined, SymbolAlreadyDefined, UndefinedSymbol, SyntaxError };

  Q_ENUM(Result)

  Assembler();
  void reset(Pass = Pass::Assembly);
  Result setOrigin(uint16_t addr);
  auto locationCounter() const { return locationCounter_; }
  const Buffer& machineCode() const { return buffer_; }
  Result assemble(InstructionType type, AddressingMode mode, int operand = 0);
  Result assemble(const QString&);
  std::optional<int> symbol(const QString&) const;

private:
  Pass pass_ = Pass::Assembly;
  bool originDefined_ = false;
  uint16_t locationCounter_ = DefaultOrigin;
  Buffer buffer_;
  std::back_insert_iterator<Buffer> iterator_;
  Symbols symbols_;

  Result addSymbol(const QString&, uint16_t);
  void addByte(uint8_t);
};
