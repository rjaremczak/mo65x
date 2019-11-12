#pragma once

#include "instruction.h"
#include <QMetaType>
#include <QString>
#include <iterator>
#include <map>
#include <vector>

class Assembler
{
  Q_GADGET

public:
  using Buffer = std::vector<uint8_t>;

  static constexpr uint16_t DefaultOrigin = 0;

  enum class Result { Ok, OriginDefined, SymbolExists, SyntaxError };
  Q_ENUM(Result)

  Assembler();
  void reset();
  Result setOrigin(uint16_t addr);
  auto locationCounter() const { return locationCounter_; }
  const Buffer& code() const { return buffer_; }
  Result assemble(InstructionType type, AddressingMode mode, int operand = 0);
  Result assemble(const QString&);
  uint16_t symbol(const QString&, uint16_t def = 0) const;

private:
  bool originDefined_ = false;
  uint16_t locationCounter_ = DefaultOrigin;
  Buffer buffer_;
  std::back_insert_iterator<Buffer> iterator_;
  std::map<QString, uint16_t> symbols_;

  Result addSymbol(const QString&, uint16_t);
  void addByte(uint8_t);
};
