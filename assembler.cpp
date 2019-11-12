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

static const QString WS("\\s+");
static const QString Label("(\\w+:)");
static const QString OpCode("([A-Z|a-z]{3})");
static const QString IndX(",[xX]\\s*");
static const QString IndY(",[yY]\\s*");
static const QString Comment("\\s*(?:;.*)?$");
static const QString ByteOp("\\$([\\d|A-H|a-z]{1,2})");
static const QString WordOp("\\$([\\d|A-H|a-z]{1,4})");
static const QString SignedByteOp("([+|-]?\\d{1,3})");

static const AddressingModeEntry AddressingModeEntries[]{
    {QRegularExpression(OpCode + Comment), NoOperands},
    {QRegularExpression(OpCode + WS + "#" + ByteOp + Comment), Immediate},
    {QRegularExpression(OpCode + WS + ByteOp + Comment), ZeroPage},
    {QRegularExpression(OpCode + WS + ByteOp + IndX + Comment), ZeroPageX},
    {QRegularExpression(OpCode + WS + ByteOp + IndY + Comment), ZeroPageY},
    {QRegularExpression(OpCode + WS + WordOp + Comment), Absolute},
    {QRegularExpression(OpCode + WS + WordOp + IndX + Comment), AbsoluteX},
    {QRegularExpression(OpCode + WS + WordOp + IndY + Comment), AbsoluteY},
    {QRegularExpression(OpCode + WS + "\\(" + WordOp + "\\)" + Comment), Indirect},
    {QRegularExpression(OpCode + WS + "\\(" + ByteOp + IndX + "\\)" + Comment), IndexedIndirectX},
    {QRegularExpression(OpCode + WS + "\\(" + ByteOp + "\\)" + IndY + Comment), IndirectIndexedY},
    {QRegularExpression(OpCode + WS + SignedByteOp + Comment), Relative}};

static const QRegularExpression OriginStatement(R"(ORG\s+\$([\d|A-H]{1,4})\s*(;.*)?$)");
static const QRegularExpression LabelRegEx(Label);

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static DetectedAddressingMode detectAddressingMode(const QString& str, int pos = 0) {
  for (auto& entry : AddressingModeEntries) {
    if (auto match = entry.pattern.match(str, pos); match.hasMatch()) { return {match, entry.mode}; }
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

bool Assembler::assemble(const QString& str) {
  bool labelDetected = false;
  int pos = 0;
  if (auto labelDef = LabelRegEx.match(str); labelDef.hasMatch()) {
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
