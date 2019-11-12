#pragma once

#include "instruction.h"
#include <QMetaType>
#include <QString>
#include <iterator>
#include <vector>

class Assembler
{
  Q_GADGET

public:
  using Buffer = std::vector<uint8_t>;

  static constexpr uint16_t DefaultOrigin = 0;

  enum class Result { Ok, OriginAlreadyDefined, InstructionNotRecognized, SyntaxError };
  Q_ENUM(Result)

  Assembler();
  void reset();
  Result setCodeOrigin(uint16_t addr);
  auto codeOrigin() const { return origin; }
  const Buffer& code() const { return buffer; }
  Result assemble(InstructionType type, AddressingMode mode = NoOperands, int operand = 0);
  Result assemble(const QString&);
  uint16_t locationCounter() { return static_cast<uint16_t>(origin + buffer.size()); }

private:
  bool originDefined = false;
  uint16_t origin = DefaultOrigin;
  Buffer buffer;
  std::back_insert_iterator<Buffer> iterator;
};
