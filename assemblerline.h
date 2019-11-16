#pragma once

#include "controlcommand.h"
#include "instruction.h"
#include "mnemonics.h"
#include <QVector>

struct AssemblerLine {
  static constexpr auto LabelGroup = 1;
  static constexpr auto OperationGroup = 2;
  static constexpr auto FirstOperandGroup = 3;

  const bool valid = false;
  const QString label;
  const QString operation;
  const QVector<QString> operands;
  const OperandsFormat operandsFormat = OperandsFormat::Implied;
  const InstructionType instructionType = InstructionType::KIL;
  const ControlCommand command = ControlCommand::None;

  AssemblerLine() = default;
  AssemblerLine(QRegularExpressionMatch match, OperandsFormat operandsFormat, ControlCommand command);
  bool isControlCommand() const { return command != ControlCommand::None; }
  bool isOperandNumeric(int num = 0) const;
  int operandAsNumber(int num = 0) const;
  uint8_t instructionSize() const;
};
