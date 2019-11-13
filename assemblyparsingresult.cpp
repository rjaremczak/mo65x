#include "assemblyparsingresult.h"

static constexpr QChar HexIndicator('$');
static constexpr QChar BinaryIndicator('%');

ParsingResult::ParsingResult(QRegularExpressionMatch match, AddressingMode mode)
    : match_(match), mode_(mode), operand_(match.captured(3) + match_.captured(4)) {
}

auto ParsingResult::size() const {
  return Instruction::sizeForAddressingMode(mode_);
}

InstructionType ParsingResult::type() const {
  const auto it =
      std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return mnemonic() == kv.second; });
  return it != MnemonicTable.end() ? it->first : InstructionType::INV;
}

bool ParsingResult::isOperandNumber() const {
  auto front = operand_.front();
  return front == HexIndicator || front == BinaryIndicator || front.isDigit() || front == '+' || front == '-';
}

int ParsingResult::operandAsNumber() const {
  auto front = operand_.front();
  if (front == HexIndicator) return operand_.rightRef(operand_.length() - 1).toInt(nullptr, 16);
  if (front == BinaryIndicator) return operand_.rightRef(operand_.length() - 1).toInt(nullptr, 2);
  return operand_.toInt(nullptr, 10);
}
