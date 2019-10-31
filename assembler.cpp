#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include "utilities.h"
#include <QRegularExpression>
#include <algorithm>

struct AddressingModeEntry {
  QRegularExpression pattern;
  AddressingMode mode;
};

struct AddressingModeInference {
  QRegularExpressionMatch match;
  AddressingMode mode;
};

const AddressingModeEntry AddressingModeEntries[]{
    {QRegularExpression("(\\w{3})\\s*$"), NoOperands},
    {QRegularExpression("(\\w{3})\\s+#\\$([\\d|a-h|A-H]{2})\\s*$"), Immediate},
    {QRegularExpression("(\\w{3})\\s+\\$([\\d|a-h|A-H]{2})\\s*$"), ZeroPage}};

static auto find(InstructionType type, AddressingMode mode) {
  return std::find_if(InstructionTable.begin(), InstructionTable.end(),
                      [=](const Instruction& ins) { return ins.type == type && ins.mode == mode; });
}

static auto findNoOperandsAddressingMode(InstructionType type) {
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](auto ins) {
    return ins.type == type && (ins.mode == Implied || ins.mode == Accumulator);
  });
  return it != InstructionTable.end() ? it->mode : Implied;
}

static AddressingModeInference inferAddressingMode(QString str) {
  for (auto& entry : AddressingModeEntries) {
    if (auto match = entry.pattern.match(str); match.hasMatch()) { return {match, entry.mode}; }
  }
  return {};
}

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

bool Assembler::enter(InstructionType type, AddressingMode mode, uint16_t operand) {
  if (type == INV) return false;

  if (mode == NoOperands) mode = findNoOperandsAddressingMode(type);

  const auto it = find(type, mode);
  if (it == InstructionTable.end()) return false;

  memory_[address_++] = static_cast<uint8_t>(std::distance(InstructionTable.begin(), it));
  if (it->size == 2) {
    memory_[address_++] = loByte(operand);
  } else if (it->size == 3) {
    memory_.write16(address_ += 2, operand & 0xffff);
  }
  return true;
}

bool Assembler::enter(QString str) {
  const auto inference = inferAddressingMode(str);
  if (!inference.match.hasMatch()) return false;

  const auto type = findInstructionType(inference.match.captured(1));
  switch (inference.mode) {
  case NoOperands: return enter(type);
  case Immediate: return enter(type, inference.mode, inference.match.captured(2).toUShort(nullptr, 16));
  case ZeroPage: return enter(type, inference.mode, inference.match.captured(2).toUShort(nullptr, 16));
  default: return false;
  }
}
