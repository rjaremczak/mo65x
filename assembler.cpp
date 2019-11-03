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
    {QRegularExpression(R"(([A-Z]{3})$)"), NoOperands},
    {QRegularExpression(R"(([A-Z]{3})\s+#\$([\d|A-H]{2})$)"), Immediate},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{2})$)"), ZeroPage},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{2})\s*,\s*X$)"), ZeroPageX},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{2})\s*,\s*Y$)"), ZeroPageY},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})$)"), Absolute},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})\s*,\s*X$)"), AbsoluteX},
    {QRegularExpression(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})\s*,\s*Y$)"), AbsoluteY},
    {QRegularExpression(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,4})\)\s*$)"), Indirect},
    {QRegularExpression(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,2}),X\)$)"), IndexedIndirectX},
    {QRegularExpression(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,2})\),Y$)"), IndirectIndexedY},
    {QRegularExpression(R"(([A-Z]{3})\s+([+|-]?\d{1,3})$)"), Relative}};

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static AddressingModeInference inferAddressingMode(const char* str) {
  const auto normalized = QString(str).toUpper();
  for (auto& entry : AddressingModeEntries) {
    if (auto match = entry.pattern.match(normalized); match.hasMatch()) { return {match, entry.mode}; }
  }
  return {};
}

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

bool Assembler::assemble(InstructionType type, AddressingMode mode, int operand) {
  if (const auto it = findInstruction(type, mode)) {
    memory_[address_++] = static_cast<uint8_t>(std::distance(InstructionTable.begin(), it));
    if (it->size > 1) memory_[address_++] = loByte(operand);
    if (it->size > 2) memory_[address_++] = hiByte(operand);
    return true;
  }
  return false;
}

bool Assembler::assemble(const char* str) {
  const auto inf = inferAddressingMode(str);
  if (!inf.match.hasMatch()) return false;

  const auto type = findInstructionType(inf.match.captured(1));
  if (inf.mode == Relative) {
    return assemble(type, inf.mode, inf.match.captured(2).toInt(nullptr, 10));
  } else if (Instruction::sizeForAddressingMode(inf.mode) > 1) {
    return assemble(type, inf.mode, inf.match.captured(2).toInt(nullptr, 16));
  } else {
    return assemble(type);
  }
}
