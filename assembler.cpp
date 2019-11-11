#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

struct AddressingModeEntry {
  QRegularExpression pattern;
  AddressingMode mode;
};

struct DetectedAddressingMode {
  QRegularExpressionMatch match;
  AddressingMode mode;
};

const AddressingModeEntry AddressingModeEntries[]{
    {QRegularExpression(R"(([A-Z|a-z]{3})$)"), NoOperands},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+#\$([\d|A-H|a-z]{1,2})$)"), Immediate},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{1,2})$)"), ZeroPage},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{1,2})\s*,\s*X$)"), ZeroPageX},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{1,2})\s*,\s*Y$)"), ZeroPageY},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{3,4})$)"), Absolute},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{3,4})\s*,\s*X$)"), AbsoluteX},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\$([\d|A-H|a-z]{3,4})\s*,\s*Y$)"), AbsoluteY},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\(\$([\d|A-H|a-z]{1,4})\)\s*$)"), Indirect},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\(\$([\d|A-H|a-z]{1,2}),X\)$)"), IndexedIndirectX},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+\(\$([\d|A-H|a-z]{1,2})\),Y$)"), IndirectIndexedY},
    {QRegularExpression(R"(([A-Z|a-z]{3})\s+([+|-]?\d{1,3})$)"), Relative}};

const QRegularExpression OriginStatement(R"(ORG\s+\$([\d|A-H]{1,4})\s*(;.*)?$)");
const QRegularExpression LabelPattern(R"(\w+:)");

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static DetectedAddressingMode detectAddressingMode(const char* str, int pos = 0) {
  const auto normalized = QString(str).toUpper();
  for (auto& entry : AddressingModeEntries) {
    if (auto match = entry.pattern.match(normalized, pos); match.hasMatch()) { return {match, entry.mode}; }
  }
  return {};
}

Assembler::Assembler() : iterator(std::back_inserter(buffer)) {
}

void Assembler::setOrigin(uint16_t addr) {
  origin = addr;
}

void Assembler::reset() {
  buffer.clear();
  iterator = std::back_inserter(buffer);
}

bool Assembler::assemble(InstructionType type, AddressingMode mode, int operand) {
  if (const auto insIt = findInstruction(type, mode)) {
    *iterator++ = static_cast<uint8_t>(std::distance(InstructionTable.begin(), insIt));
    if (insIt->size > 1) *iterator++ = uint8_t(operand);
    if (insIt->size > 2) *iterator++ = uint8_t(operand >> 8);
    return true;
  }
  return false;
}

bool Assembler::assemble(const char* str) {
  bool labelDetected = false;
  int pos = 0;
  if (auto labelDef = LabelPattern.match(str); labelDef.hasMatch()) {
    labelDetected = true;
    pos = labelDef.capturedEnd();
  }

  if (auto org = OriginStatement.match(str, pos); org.hasMatch()) {
    setOrigin(org.captured(1).toUShort(nullptr, 16));
    return true;
  }

  const auto inf = detectAddressingMode(str, pos);
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
