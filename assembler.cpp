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
    {QRegularExpression("(\\w{3})\\s+\\$([\\d|a-h|A-H]{2})\\s*$"), ZeroPage},
    {QRegularExpression(R"((\w{3})\s+\$([\d|a-h|A-H]{2})\s*,\s*X\s*$)"), ZeroPageX},
    {QRegularExpression(R"((\w{3})\s+\$([\d|a-h|A-H]{2})\s*,\s*Y\s*$)"), ZeroPageY}};

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static AddressingModeInference inferAddressingMode(QString str) {
  for (auto& entry : AddressingModeEntries) {
    if (auto match = entry.pattern.match(str); match.hasMatch()) { return {match, entry.mode}; }
  }
  return {};
}

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

bool Assembler::assemble(InstructionType type, AddressingMode mode, uint16_t operand) {
  if (const auto it = findInstruction(type, mode)) {
    memory_[address_++] = static_cast<uint8_t>(std::distance(InstructionTable.begin(), it));
    if (it->size > 1) memory_[address_++] = loByte(operand);
    if (it->size > 2) memory_[address_++] = hiByte(operand);
    return true;
  }
  return false;
}

bool Assembler::assemble(QString str) {
  const auto inf = inferAddressingMode(str);
  if (!inf.match.hasMatch()) return false;

  const auto type = findInstructionType(inf.match.captured(1));
  switch (inf.mode) {
  case NoOperands: return assemble(type);
  case Immediate:
  case ZeroPage:
  case ZeroPageX:
  case ZeroPageY: return assemble(type, inf.mode, inf.match.captured(2).toUShort(nullptr, 16));
  default: return false;
  }
}
