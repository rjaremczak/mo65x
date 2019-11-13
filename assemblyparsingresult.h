#pragma once

#include "instruction.h"
#include "mnemonics.h"
#include <QRegularExpression>

class ParsingResult {
public:
  ParsingResult() = default;
  ParsingResult(QRegularExpressionMatch match, AddressingMode mode);
  bool valid() const { return !match_.hasMatch() || type() != InstructionType::INV; }
  auto label() const { return match_.captured(1); }
  auto mnemonic() const { return match_.captured(2); }
  auto addrMode() const { return mode_; }
  auto operand() const { return operand_; }
  auto size() const;
  InstructionType type() const;
  bool isOperandNumber() const;
  int operandAsNumber() const;

private:
  QRegularExpressionMatch match_;
  const AddressingMode mode_ = AddressingMode::NoOperands;
  const QString operand_;
};
