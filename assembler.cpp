#include "assembler.h"
#include "assemblyparsingresult.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

struct AssemblyLine {
  QRegularExpression regex;
  AddressingMode mode;

  AssemblyLine(const QString& pattern, AddressingMode mode) : regex(pattern), mode(mode) {}
};

static const QString Label("(\\w{2,})");
static const QString LabelDef("^(?:" + Label + ":)?\\s*");
static const QString Mnemonic("([A-Z|a-z]{3})\\s*");
static const QString IndX(",[xX]\\s*");
static const QString IndY(",[yY]\\s*");
static const QString Comment("(?:;.*)?$");
static const QString ByteOp("([\\$|\\%]?[\\d|A-H|a-z]{1,2})\\s*");
static const QString WordOp("([\\$|\\%]?[\\d|A-H|a-z]{1,4})\\s*");
static const QString RelativeOp("(?:([+|-]?\\d{1,3})|" + Label + ")\\s*");

const AssemblyLine LineParsersTable[]{{LabelDef + Mnemonic + Comment, NoOperands},
                                      {LabelDef + Mnemonic + "#" + ByteOp + Comment, Immediate},
                                      {LabelDef + Mnemonic + ByteOp + Comment, ZeroPage},
                                      {LabelDef + Mnemonic + ByteOp + IndX + Comment, ZeroPageX},
                                      {LabelDef + Mnemonic + ByteOp + IndY + Comment, ZeroPageY},
                                      {LabelDef + Mnemonic + WordOp + Comment, Absolute},
                                      {LabelDef + Mnemonic + WordOp + IndX + Comment, AbsoluteX},
                                      {LabelDef + Mnemonic + WordOp + IndY + Comment, AbsoluteY},
                                      {LabelDef + Mnemonic + "\\(" + WordOp + "\\)" + Comment, Indirect},
                                      {LabelDef + Mnemonic + "\\(" + ByteOp + IndX + "\\)" + Comment, IndexedIndirectX},
                                      {LabelDef + Mnemonic + "\\(" + ByteOp + "\\)" + IndY + Comment, IndirectIndexedY},
                                      {LabelDef + Mnemonic + RelativeOp + Comment, Relative}};

static const QRegularExpression EmptyLine(LabelDef + Comment);
static const QRegularExpression OriginStatement(R"(^\s*ORG\s+\$([\d|A-H]{1,4})\s*(;.*)?$)");

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static ParsingResult parseAssemblyLine(const QString& str) {
  for (auto& entry : LineParsersTable) {
    if (auto match = entry.regex.match(str); match.hasMatch()) { return {match, entry.mode}; }
  }
  return {};
}

Assembler::Assembler() : iterator_(std::back_inserter(code_)) {
}

void Assembler::reset(Pass pass) {
  pass_ = pass;
  originDefined_ = false;
  origin_ = DefaultOrigin;
  locationCounter_ = DefaultOrigin;
  iterator_ = std::back_inserter(code_);
  if (pass == Pass::Assembly) code_.clear();
  if (pass == Pass::ScanForSymbols) symbols_.clear();
}

Assembler::Result Assembler::setOrigin(uint16_t addr) {
  if (originDefined_) return Result::OriginAlreadyDefined;

  originDefined_ = true;
  origin_ = addr;
  locationCounter_ = addr;
  return Result::Ok;
}

Assembler::Result Assembler::assemble(const QString& str) {
  if (const auto re = OriginStatement.match(str); re.hasMatch()) { return setOrigin(re.captured(1).toUShort(nullptr, 16)); }
  if (const auto re = EmptyLine.match(str); re.hasMatch()) {
    if (const auto& label = re.captured(1); !label.isNull()) return addSymbol(label, locationCounter_);
    return Result::Ok;
  }

  const auto line = parseAssemblyLine(str);
  if (!line.valid()) return Result::SyntaxError;
  if (!line.label().isEmpty()) {
    if (auto res = addSymbol(line.label(), locationCounter_); res != Result::Ok) return res;
  }

  if (line.addrMode() == NoOperands) return assemble(line.type(), NoOperands);
  if (line.operand().isEmpty()) {
    // test
    return Result::MissingOperand;
  }

  int operand;
  if (line.isOperandNumber()) {
    operand = line.operandAsNumber();
  } else if (auto opt = symbol(line.operand())) {
    if (line.addrMode() == Relative) {
      operand = *opt - locationCounter_ - line.size();
    } else {
      operand = *opt;
    }
  } else {
    return Result::UndefinedSymbol;
  }

  return assemble(line.type(), line.addrMode(), operand);
}

Assembler::Result Assembler::assemble(InstructionType type, AddressingMode mode, int operand) {
  if (const auto insIt = findInstruction(type, mode)) {
    addByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), insIt)));
    if (insIt->size > 1) addByte(static_cast<uint8_t>(operand));
    if (insIt->size > 2) addByte(static_cast<uint8_t>(operand >> 8));
    return Result::Ok;
  }
  return Result::SyntaxError;
}

std::optional<int> Assembler::symbol(const QString& name) const {
  if (const auto it = symbols_.find(name); it != symbols_.end()) return it->second;
  return std::nullopt;
}

Assembler::Result Assembler::addSymbol(const QString& name, uint16_t value) {
  if (pass_ == Pass::ScanForSymbols) {
    if (symbols_.find(name) != symbols_.end()) return Result::SymbolAlreadyDefined;
    symbols_[name] = value;
  }
  return Result::Ok;
}

void Assembler::addByte(uint8_t b) {
  if (pass_ == Pass::Assembly) *iterator_++ = b;
  locationCounter_++;
}
