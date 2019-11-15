#pragma once

#include "instruction.h"
#include "mnemonics.h"
#include <QRegularExpression>

enum class Command { None, SetOrigin, EmitByte };

class AssemblerLine {
public:
  AssemblerLine() = default;
  AssemblerLine(QRegularExpressionMatch match, OperandsFormat operandsFormat, Command command);
  AssemblerLine(QRegularExpressionMatch match, Command command);
  bool invalid() const { return !match_.hasMatch(); }
  bool isCommand() const { return command_ != Command::None; }
  bool isInstruction() const { return instructionType_ != InstructionType::INV; }
  auto label() const { return match_.captured(1); }
  auto operation() const { return match_.captured(2); }
  auto addrMode() const { return operandsFormat_; }
  auto operand() const { return operand_; }
  uint8_t size() const;
  InstructionType instruction() const { return instructionType_; }
  Command command() const { return command_; }
  bool isOperandNumber() const;
  int operandAsNumber() const;

private:
  QRegularExpressionMatch match_;
  const QString operation_;
  const OperandsFormat operandsFormat_ = OperandsFormat::Implied;
  const InstructionType instructionType_ = InstructionType::INV;
  const Command command_ = Command::None;
  const QString operand_;
};
