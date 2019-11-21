#include "assemblerline.h"
#include <QRegularExpressionMatch>

static constexpr QChar HexPrefix('$');
static constexpr QChar BinPrefix('%');

static InstructionType resolveInstructionType(const QString& operation) {
  const auto it =
      std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return operation == kv.second; });
  return it != MnemonicTable.end() ? it->first : InstructionType::KIL;
}

AssemblerLine::AssemblerLine(QRegularExpressionMatch match, OperandsFormat operandsFormat, ControlCommand command)
    : valid(match.hasMatch()), label(match.captured(LabelGroup)), operation(match.captured(OperationGroup).toUpper()),
      operands(match.capturedTexts().mid(FirstOperandGroup).toVector()), operandsFormat(operandsFormat),
      instructionType(command == ControlCommand::None ? resolveInstructionType(operation) : InstructionType::KIL),
      command(command) {
}

uint8_t AssemblerLine::instructionSize() const {
  return Instruction::sizeForAddressingMode(operandsFormat);
}

bool AssemblerLine::isOperandNumeric(int num) const {
  const auto front = operands[num].front();
  return front == HexPrefix || front == BinPrefix || front.isDigit() || front == '+' || front == '-';
}

int AssemblerLine::operandAsNumber(int num) const {
  const auto op = operands[num];
  const auto front = op.front();
  if (front == HexPrefix) return op.right(op.length() - 1).toInt(nullptr, 16);
  if (front == BinPrefix) return op.right(op.length() - 1).toInt(nullptr, 2);
  return op.toInt(nullptr, 10);
}
