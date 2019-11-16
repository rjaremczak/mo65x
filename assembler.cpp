#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QRegularExpression>
#include <algorithm>

struct AssemblerLinePattern {
  const QRegularExpression regex;
  const OperandsFormat operandsFormat;
  const ControlCommand command;

  AssemblerLinePattern(const QString& pattern, OperandsFormat operandsFormat)
      : regex(pattern, QRegularExpression::CaseInsensitiveOption), operandsFormat(operandsFormat), command(ControlCommand::None) {
  }

  AssemblerLinePattern(const QString& pattern, ControlCommand command)
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
static const QString OriginCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const QString ByteCmd("(\\.(?:BYTE))\\s+");

const AssemblerLinePattern LinePatternsTable[]{{LabelDef + OriginCmd + OpWord + Comment, ControlCommand::DefineOrigin},
                                               {LabelDef + ByteCmd + OpByte + "+", ControlCommand::EmitBytes},
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

void Assembler::resetOrigin(uint16_t addr) {
  originDefined_ = false;
  origin_ = addr;
  locationCounter_ = addr;
}

Assembler::Result Assembler::defineOrigin(uint16_t addr) {
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
  if (!line.valid) return Result::SyntaxError;
  if (!line.label.isEmpty()) {
    if (auto res = addSymbol(line.label, locationCounter_); res != Result::Ok) return res;
  }
  if (line.isControlCommand()) return processControlCommand(line);
  return processInstruction(line);
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

Assembler::Result Assembler::processControlCommand(const AssemblerLine& line) {
  switch (line.command) {
  case ControlCommand::DefineOrigin: return cmdSetOrigin(line);
  case ControlCommand::EmitBytes: return cmdEmitByte(line);
  default: return Result::CommandProcessingError;
  }
}

Assembler::Result Assembler::processInstruction(const AssemblerLine& line) {
  if (line.operandsFormat == NoOperands) return assemble(line.instructionType, NoOperands);
  if (line.operands.isEmpty()) return Result::MissingOperand;

  int num;
  if (line.isOperandNumeric()) {
    num = line.operandAsNumber();
  } else if (auto opt = symbol(line.operands[0])) {
    if (line.operandsFormat == Branch) {
      num = *opt - locationCounter_ - line.instructionSize();
    } else {
      num = *opt;
    }
  } else {
    return Result::UndefinedSymbol;
  }

  return assemble(line.instructionType, line.operandsFormat, num);
}

Assembler::Result Assembler::cmdSetOrigin(const AssemblerLine& line) {
  if (line.isOperandNumeric()) {
    return defineOrigin(static_cast<uint16_t>(line.operandAsNumber()));
  } else {
    return Result::NumericOperandRequired;
  }
}

Assembler::Result Assembler::cmdEmitByte(const AssemblerLine& line) {
  for (auto num = 0; num < line.operands.size(); num++) {
    if (!line.isOperandNumeric(num)) return Result::NumericOperandRequired;
    addByte(static_cast<uint8_t>(line.operandAsNumber(num)));
  }
  return Result::Ok;
}

Assembler::Result Assembler::addSymbol(const QString& name, uint16_t value) {
  if (mode_ == Mode::ScanForSymbols) {
    if (symbols_.find(name) != symbols_.end()) return Result::SymbolAlreadyDefined;
    symbols_[name] = value;
  }
  return Result::Ok;
}

void Assembler::addByte(uint8_t b) {
  if (mode_ == Mode::EmitCode) *iterator_++ = b;
  locationCounter_++;
}
