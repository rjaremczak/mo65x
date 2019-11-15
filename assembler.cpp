#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

struct AssemblerLinePattern {
  const QRegularExpression regex;
  const OperandsFormat operandsFormat;
  const Command command;

  AssemblerLinePattern(const QString& pattern, OperandsFormat operandsFormat)
      : regex(pattern, QRegularExpression::CaseInsensitiveOption), operandsFormat(operandsFormat), command(Command::None) {}

  AssemblerLinePattern(const QString& pattern, Command command)
      : regex(pattern, QRegularExpression::CaseInsensitiveOption), operandsFormat(Implied), command(command) {}
};

static const QString Label("[a-z]\\w+");
static const QString HexByte("\\$[\\d|a-h]{1,2}");
static const QString HexWord("\\$[\\d|a-h]{1,4}");
static const QString BinByte("\\%[01]{8}");
static const QString BinWord("\\%[01]{16}");
static const QString DecByte("\\d{1,3}");
static const QString DecWord("\\d{1,5}");
static const QString LabelDef("^(?:(" + Label + "):)?\\s*");
static const QString OpType("([a-z]{3})\\s*");
static const QString IndX(",x\\s*");
static const QString IndY(",y\\s*");
static const QString Comment("(?:;.*)?$");
static const QString OpByte("((?:" + HexByte + ")|(?:" + BinByte + ")|(?:" + DecByte + "))\\s*");
static const QString OpWord("((?:" + HexWord + ")|(?:" + BinWord + ")|(?:" + DecWord + "))\\s*");
static const QString OpOffset("((?:[+|-]?\\d{1,3})|(?:" + Label + "))\\s*");
static const QString OriginCmd("((?:\\.?ORG\\s+)|(?:\\*\\s*\\=\\s*))");

const AssemblerLinePattern LinePatternsTable[]{{LabelDef + OriginCmd + OpWord + Comment, Command::SetOrigin},
                                               {LabelDef + OpType + Comment, NoOperands},
                                               {LabelDef + OpType + "#" + OpByte + Comment, Immediate},
                                               {LabelDef + OpType + OpByte + Comment, ZeroPage},
                                               {LabelDef + OpType + OpByte + IndX + Comment, ZeroPageX},
                                               {LabelDef + OpType + OpByte + IndY + Comment, ZeroPageY},
                                               {LabelDef + OpType + OpWord + Comment, Absolute},
                                               {LabelDef + OpType + OpWord + IndX + Comment, AbsoluteX},
                                               {LabelDef + OpType + OpWord + IndY + Comment, AbsoluteY},
                                               {LabelDef + OpType + "\\(" + OpWord + "\\)" + Comment, Indirect},
                                               {LabelDef + OpType + "\\(" + OpByte + IndX + "\\)" + Comment, IndexedIndirectX},
                                               {LabelDef + OpType + "\\(" + OpByte + "\\)" + IndY + Comment, IndirectIndexedY},
                                               {LabelDef + OpType + OpOffset + Comment, Branch}};

static const QRegularExpression EmptyLine(LabelDef + Comment, QRegularExpression::CaseInsensitiveOption);

static const Instruction* findInstruction(InstructionType type, OperandsFormat mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static AssemblerLine parseAssemblyLine(const QString& str) {
  for (auto& entry : LinePatternsTable) {
    if (auto match = entry.regex.match(str); match.hasMatch()) {
      return AssemblerLine(match, entry.operandsFormat, entry.command);
    }
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

uint8_t Assembler::lastInstructionByte(size_t num) const {
  return code_[std::min(lastInstructionIndex_ + num, code_.size() - 1)];
}

Assembler::Result Assembler::process(const QString& str) {
  if (const auto re = EmptyLine.match(str); re.hasMatch()) {
    if (const auto& label = re.captured(1); !label.isNull()) return addSymbol(label, locationCounter_);
    return Result::Ok;
  }

  const auto line = parseAssemblyLine(str);
  if (line.invalid()) return Result::SyntaxError;
  if (!line.label().isEmpty()) {
    if (auto res = addSymbol(line.label(), locationCounter_); res != Result::Ok) return res;
  }
  if (line.isCommand()) return processCommand(line);
  if (line.isInstruction()) return processInstruction(line);
  return Result::FatalProcessingError;
}

Assembler::Result Assembler::assemble(InstructionType type, OperandsFormat mode, int operand) {
  lastInstructionIndex_ = code_.size();
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

Assembler::Result Assembler::processCommand(const AssemblerLine& line) {
  switch (line.command()) {
  case Command::SetOrigin:
    if (line.isOperandNumber()) {
      return setOrigin(static_cast<uint16_t>(line.operandAsNumber()));
    } else {
      return Result::NumericOperandRequired;
    }
  case Command::EmitByte: break;
  default: break;
  }
  return Result::CommandProcessingError;
}

Assembler::Result Assembler::processInstruction(const AssemblerLine& line) {
  if (line.addrMode() == NoOperands) return assemble(line.instruction(), NoOperands);
  if (line.operand().isEmpty()) return Result::MissingOperand;

  int operand;
  if (line.isOperandNumber()) {
    operand = line.operandAsNumber();
  } else if (auto opt = symbol(line.operand())) {
    if (line.addrMode() == Branch) {
      operand = *opt - locationCounter_ - line.size();
    } else {
      operand = *opt;
    }
  } else {
    return Result::UndefinedSymbol;
  }

  return assemble(line.instruction(), line.addrMode(), operand);
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
