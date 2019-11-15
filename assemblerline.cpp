#include "assemblerline.h"

static constexpr QChar HexIndicator('$');
static constexpr QChar BinaryIndicator('%');

static InstructionType resolveInstructionType(const QString& operation) {
  const auto it =
      std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return operation == kv.second; });
  return it != MnemonicTable.end() ? it->first : InstructionType::INV;
}

AssemblerLine::AssemblerLine(QRegularExpressionMatch match, OperandsFormat operandsFormat, Command command)
    : match_(match), operation_(match.captured(2)), operandsFormat_(operandsFormat),
      instructionType_(command == Command::None ? resolveInstructionType(operation_) : InstructionType::INV), command_(command),
      operand_(match.captured(3)) {
}

uint8_t AssemblerLine::size() const {
  return Instruction::sizeForAddressingMode(operandsFormat_);
}

bool AssemblerLine::isOperandNumber() const {
  auto front = operand_.front();
  return front == HexIndicator || front == BinaryIndicator || front.isDigit() || front == '+' || front == '-';
}

int AssemblerLine::operandAsNumber() const {
  auto front = operand_.front();
  if (front == HexIndicator) return operand_.rightRef(operand_.length() - 1).toInt(nullptr, 16);
  if (front == BinaryIndicator) return operand_.rightRef(operand_.length() - 1).toInt(nullptr, 2);
  return operand_.toInt(nullptr, 10);
}
