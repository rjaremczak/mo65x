#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

struct AssemblyLine {
  QRegularExpression regex;
  AddressingMode mode;

  AssemblyLine(const QString& pattern, AddressingMode mode) : regex(pattern), mode(mode) {}
};

class ParsingResult {
public:
  ParsingResult() = default;
  ParsingResult(QRegularExpressionMatch match, AddressingMode mode) : match(match), mode(mode) {}
  bool valid() const { return !match.hasMatch() || type() != InstructionType::INV; }
  auto label() const { return match.captured(1); }
  auto mnemonic() const { return match.captured(2); }
  auto operand() const { return match.captured(3); }
  auto addrMode() const { return mode; }
  auto size() const { return Instruction::sizeForAddressingMode(mode); }
  InstructionType type() const {
    const auto it = std::find_if(MnemonicTable.begin(), MnemonicTable.end(),
                                 [=](const auto& kv) { return mnemonic() == QString(kv.second); });
    return it != MnemonicTable.end() ? it->first : InstructionType::INV;
  }

private:
  QRegularExpressionMatch match;
  AddressingMode mode;
};

static const QString Label("^(?:(\\w{2,}):)?\\s*");
static const QString Mnemonic("([A-Z|a-z]{3})\\s*");
static const QString IndX(",[xX]\\s*");
static const QString IndY(",[yY]\\s*");
static const QString Comment("(?:;.*)?$");
static const QString ByteOp("\\$([\\d|A-H|a-z]{1,2})");
static const QString WordOp("\\$([\\d|A-H|a-z]{1,4})");
static const QString SignedByteOp("([+|-]?\\d{1,3})");

const AssemblyLine LineParsersTable[]{{Label + Mnemonic + Comment, NoOperands},
                                      {Label + Mnemonic + "#" + ByteOp + Comment, Immediate},
                                      {Label + Mnemonic + ByteOp + Comment, ZeroPage},
                                      {Label + Mnemonic + ByteOp + IndX + Comment, ZeroPageX},
                                      {Label + Mnemonic + ByteOp + IndY + Comment, ZeroPageY},
                                      {Label + Mnemonic + WordOp + Comment, Absolute},
                                      {Label + Mnemonic + WordOp + IndX + Comment, AbsoluteX},
                                      {Label + Mnemonic + WordOp + IndY + Comment, AbsoluteY},
                                      {Label + Mnemonic + "\\(" + WordOp + "\\)" + Comment, Indirect},
                                      {Label + Mnemonic + "\\(" + ByteOp + IndX + "\\)" + Comment, IndexedIndirectX},
                                      {Label + Mnemonic + "\\(" + ByteOp + "\\)" + IndY + Comment, IndirectIndexedY},
                                      {Label + Mnemonic + SignedByteOp + Comment, Relative}};

static const QRegularExpression EmptyLine(Label + Comment);
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

Assembler::Assembler() : iterator_(std::back_inserter(buffer_)) {
}

void Assembler::reset() {
  originDefined_ = false;
  locationCounter_ = DefaultOrigin;
  buffer_.clear();
  symbols_.clear();
  iterator_ = std::back_inserter(buffer_);
}

Assembler::Result Assembler::setOrigin(uint16_t addr) {
  if (originDefined_) return Result::OriginDefined;

  locationCounter_ = addr;
  originDefined_ = true;
  return Result::Ok;
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

Assembler::Result Assembler::assemble(const QString& str) {
  if (const auto re = OriginStatement.match(str); re.hasMatch()) { return setOrigin(re.captured(1).toUShort(nullptr, 16)); }
  if (const auto re = EmptyLine.match(str); re.hasMatch()) {
    if (const auto& label = re.captured(1); !label.isNull()) return addSymbol(label, locationCounter_);
    return Result::Ok;
  }

  const auto line = parseAssemblyLine(str);
  if (!line.valid()) return Result::SyntaxError;
  if (!line.label().isNull()) {
    if (auto res = addSymbol(line.label(), locationCounter_); res != Result::Ok) return res;
  }
  if (line.addrMode() == Relative) return assemble(line.type(), line.addrMode(), line.operand().toInt(nullptr, 10));
  if (line.size() > 1) return assemble(line.type(), line.addrMode(), line.operand().toInt(nullptr, 16));
  return assemble(line.type(), AddressingMode::NoOperands);
}

uint16_t Assembler::symbol(const QString& name, uint16_t def) const {
  if (const auto it = symbols_.find(name); it != symbols_.end()) return it->second;
  return def;
}

Assembler::Result Assembler::addSymbol(const QString& name, uint16_t value) {
  if (symbols_.find(name) != symbols_.end()) return Result::SymbolExists;
  symbols_[name] = value;
  return Result::Ok;
}

void Assembler::addByte(uint8_t b) {
  *iterator_++ = b;
  locationCounter_++;
}
