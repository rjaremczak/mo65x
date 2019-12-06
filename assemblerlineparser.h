#pragma once

#include "controlcommand.h"
#include "instruction.h"
#include "operandsformat.h"
#include <QRegularExpressionMatch>
#include <QString>

class AssemblerLineParser
{
public:
  enum Format {
    Invalid,
    Empty,
    CmdOrg,
    CmdByte,
    CmdWord,
    InsNoOperands,
    InsImmediate,
    InsAbsolute,
    InsAbsoluteIndexedX,
    InsAbsoluteIndexedY,
    InsIndirect,
    InsIndexedIndirectX,
    InsIndirectIndexedY,
    InsBranch
  };

  static constexpr auto LabelGroup = 1;
  static constexpr auto OperationGroup = 2;
  static constexpr auto FirstOperandGroup = 3;

  const bool valid = false;
  const QString label;
  const QString mnemonic;
  const QString operand;
  const OperandsFormat operandsFormat = OperandsFormat::Implied;
  const InstructionType instructionType = InstructionType::KIL;
  const ControlCommand command = ControlCommand::None;

  static AssemblerLineParser parse(const QString& line);
  bool isOperandNumber() const;
  int operandAsNumber();
  QString operandAsSymbol();

private:
  AssemblerLineParser() = default;
  AssemblerLineParser(QRegularExpressionMatch match, Format format);

  const QRegularExpressionMatch match;
  const Format format = Invalid;
};
