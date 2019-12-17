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

static constexpr auto LabelGroup = 1;
static constexpr auto OperationGroup = 2;
static constexpr auto FirstOperandGroup = 3;
static constexpr QChar HexPrefix('$');
static constexpr QChar BinPrefix('%');

static const QString Symbol("[a-z]\\w+");
static const QString HexByte("\\$[\\d|a-h]{1,2}");
static const QString HexWord("\\$[\\d|a-h]{1,4}");
static const QString BinByte("\\%[01]{8}");
static const QString BinWord("\\%[01]{16}");
static const QString DecByte("\\d{1,3}");
static const QString DecWord("\\d{1,5}");
static const QString Label("^(?:(" + Symbol + "):)?\\s*");
static const QString OpType("([a-z]{3})\\s*");
static const QString IndX(",x\\s*");
static const QString IndY(",y\\s*");
static const QString Comment("(?:;.*)?$");
static const QString OpByte("((?:" + HexByte + ")|(?:" + BinByte + ")|(?:" + DecByte + "))\\s*");
static const QString OpWord("((?:" + HexWord + ")|(?:" + BinWord + ")|(?:" + DecWord + "))\\s*");
static const QString OpOffset("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");
static const QString OriginCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const QString ByteCmd("(\\.(?:BYTE))\\s+");
static const QString WordCmd("(\\.(?:WORD))\\s+");

static const QString HexNum("\\$[\\d|a-h]{1,4}");
static const QString DecNum("\\d{1,5}");
static const QString BinNum("\\%[01]{1,16}");
static const QString Mnemonic("([a-z]{3})\\s*");
static const QString Operand("((?:" + HexNum + ")|(?:" + DecNum + ")|(?:" + BinNum + ")|(?:" + Symbol + "))\\s*");
static const QString OrgCmd("((?:\\.ORG\\s+)|(?:\\*\\s*\\=\\s*))");
static const QString BranchTarget("((?:[+|-]?\\d{1,3})|(?:" + Symbol + "))\\s*");

const AssemblerLinePattern LinePatternsTable[]{{Label + OriginCmd + OpWord + Comment, ControlCommand::SetOrigin},
                                               {Label + ByteCmd + OpByte + "+", ControlCommand::EmitBytes},
                                               {Label + WordCmd + OpWord + "+", ControlCommand::EmitWords},
                                               {Label + OpType + Comment, NoOperands},
                                               {Label + OpType + "#" + OpByte + Comment, Immediate},
                                               {Label + OpType + OpByte + Comment, ZeroPage},
                                               {Label + OpType + OpByte + IndX + Comment, ZeroPageX},
                                               {Label + OpType + OpByte + IndY + Comment, ZeroPageY},
                                               {Label + OpType + OpWord + Comment, Absolute},
                                               {Label + OpType + OpWord + IndX + Comment, AbsoluteX},
                                               {Label + OpType + OpWord + IndY + Comment, AbsoluteY},
                                               {Label + OpType + "\\(" + OpWord + "\\)" + Comment, Indirect},
                                               {Label + OpType + "\\(" + OpByte + IndX + "\\)" + Comment, IndexedIndirectX},
                                               {Label + OpType + "\\(" + OpByte + "\\)" + IndY + Comment, IndirectIndexedY},
                                               {Label + OpType + OpOffset + Comment, Branch}};

static const QRegularExpression EmptyLine(Label + Comment, QRegularExpression::CaseInsensitiveOption);

static QRegularExpression regex(const QString pattern) {
  return QRegularExpression(Label + pattern + "\\s*" + Comment, QRegularExpression::CaseInsensitiveOption);
}

const Assembler::PatternEntry Assembler::Patterns[]{
    {regex(""), &Assembler::handleEmpty},
    {regex(OrgCmd + Operand), &Assembler::handleOrigin},
    {regex(ByteCmd + "(?:[\\s\\,]" + Operand + ")+"), &Assembler::handleByte},
    {regex(WordCmd), &Assembler::handleWord},
    {regex(Mnemonic), &Assembler::handleImpliedOrAccumulator},
    {regex(Mnemonic + "#" + Operand), &Assembler::handleImmediate},
    {regex(Mnemonic + Operand), &Assembler::handleAbsolute},
    {regex(Mnemonic + Operand + ",x"), &Assembler::handleAbsoluteIndexedX},
    {regex(Mnemonic + Operand + ",y"), &Assembler::handleAbsoluteIndexedY},
    {regex(Mnemonic + "\\(" + Operand + "\\)"), &Assembler::handleIndirect},
    {regex(Mnemonic + "\\(" + Operand + ",x\\)"), &Assembler::handleIndexedIndirectX},
    {regex(Mnemonic + "\\(" + Operand + "\\),y"), &Assembler::handleIndirectIndexedY},
    {regex(Mnemonic + BranchTarget), &Assembler::handleBranch}};

static InstructionType resolveInstructionType(const QString& str) {
  const auto it = std::find_if(MnemonicTable.begin(), MnemonicTable.end(), [=](const auto& kv) { return str == kv.second; });
  if (it == MnemonicTable.end()) throw AssemblyResult::InvalidMnemonic;
  return it->first;
}

static const Instruction* resolveInstruction(const QString& str, OperandsFormat mode) {
  const auto type = resolveInstructionType(str);
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(),
                               [=](const Instruction& ins) { return ins.type == type && ins.mode == mode; });
  if (it == InstructionTable.end()) throw AssemblyResult::InvalidInstructionFormat;
  return it;
}

static const Instruction* findInstruction(InstructionType type, OperandsFormat mode) {
  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static AssemblerLine parseLine(const QString& str) {
  for (auto& entry : LinePatternsTable) {
    if (auto match = entry.regex.match(str); match.hasMatch()) {
      return AssemblerLine(match, entry.operandsFormat, entry.command);
    }
  }
  return {};
}

template <typename T>
T safeCast(int n) {
  if (n >= std::numeric_limits<T>::min() && n <= std::numeric_limits<T>::max()) return static_cast<T>(n);
  throw AssemblyResult::ValueOutOfRange;
}

static bool isNumber(const QString& str) {
  const auto front = str.front();
  return front == HexPrefix || front == BinPrefix || front.isDigit() || front == '+' || front == '-';
}

static int parseNumber(const QString& str) {
  const auto op = str;
  const auto front = op.front();
  if (front == HexPrefix) return op.right(op.length() - 1).toInt(nullptr, 16);
  if (front == BinPrefix) return op.right(op.length() - 1).toInt(nullptr, 2);
  return op.toInt(nullptr, 10);
}

Assembler::Assembler(Memory& memory) : memory(memory) {
}

void Assembler::init(uint16_t addr) {
  locationCounter = addr;
  written = 0;
}

void Assembler::clearSymbols() {
  symbolTable.clear();
}

void Assembler::changeMode(Assembler::ProcessingMode mode) {
  this->mode = mode;
}

AssemblyResult Assembler::processLine(const QString& str) {
  lastLocationCounter = locationCounter;

  if (const auto re = EmptyLine.match(str); re.hasMatch()) {
    if (const auto& label = re.captured(1); !label.isNull()) return defineSymbol(label, locationCounter);
    return AssemblyResult::Ok;
  }

  const auto line = parseLine(str);
  if (!line.valid) return AssemblyResult::SyntaxError;
  if (!line.label.isEmpty()) {
    if (auto res = defineSymbol(line.label, locationCounter); res != AssemblyResult::Ok) return res;
  }
  return line.isControlCommand() ? processControlCommand(line) : processInstruction(line);
}

void Assembler::processLine2(const QString& str) {
  lastLocationCounter = locationCounter;
  for (auto& entry : Patterns) {
    if (auto match = entry.regex.match(str); match.hasMatch()) {
      if (auto label = match.captured(LabelGroup); !label.isEmpty()) defineSymbol2(label, locationCounter);
      (this->*entry.handler)(match);
    }
  }
}

AssemblyResult Assembler::assemble(InstructionType type, OperandsFormat mode, int operand) {
  if (const auto insIt = findInstruction(type, mode)) {
    addByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), insIt)));
    if (insIt->size > 1) addByte(static_cast<uint8_t>(operand));
    if (insIt->size > 2) addByte(static_cast<uint8_t>(operand >> 8));
    return AssemblyResult::Ok;
  }
  return AssemblyResult::SyntaxError;
}

AddressRange Assembler::affectedAddressRange() const {
  return addressRange;
}

size_t Assembler::bytesWritten() const {
  return written;
}

int Assembler::operandValue(const QString& str) {
  if (isNumber(str)) return parseNumber(str);
  if (const auto& opt = symbolTable.get(str)) return *opt;
  throw AssemblyResult::SymbolNotDefined;
}

int8_t Assembler::operandAsBranchDisplacement(const QString& str) {
  if (isNumber(str)) return safeCast<int8_t>(operandValue(str));
  if (const auto& opt = symbolTable.get(str)) return safeCast<int8_t>(*opt - locationCounter - 2);
  throw AssemblyResult::SymbolNotDefined;
}

void Assembler::handleEmpty(const QRegularExpressionMatch&) {
}

void Assembler::handleOrigin(const QRegularExpressionMatch& match) {
  locationCounter = safeCast<uint16_t>(operandValue(match.captured(FirstOperandGroup)));
}

void Assembler::handleByte(const QRegularExpressionMatch& match) {
  for (auto i = FirstOperandGroup; i <= match.lastCapturedIndex(); i++) {
    addByte(safeCast<uint8_t>(operandValue(match.captured(i))));
  }
}

void Assembler::handleWord(const QRegularExpressionMatch& match) {
  for (auto i = FirstOperandGroup; i <= match.lastCapturedIndex(); i++) {
    auto value = safeCast<uint16_t>(operandValue(match.captured(i)));
    addByte(static_cast<uint8_t>(value));
    addByte(static_cast<uint8_t>(value >> 8));
  }
}

void Assembler::handleImpliedOrAccumulator(const QRegularExpressionMatch& match) {
  assemble(match, OperandsFormat::Implied);
}

void Assembler::handleImmediate(const QRegularExpressionMatch& match) {
  assemble(match, OperandsFormat::Immediate, safeCast<uint8_t>(operandValue(match.captured(FirstOperandGroup))));
}

void Assembler::handleAbsolute(const QRegularExpressionMatch& match) {
  auto addr = safeCast<Address>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, addr < 256 ? OperandsFormat::ZeroPage : OperandsFormat::Absolute, addr);
}

void Assembler::handleAbsoluteIndexedX(const QRegularExpressionMatch& match) {
  auto addr = safeCast<Address>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, addr < 256 ? OperandsFormat::ZeroPageX : OperandsFormat::AbsoluteX, addr);
}

void Assembler::handleAbsoluteIndexedY(const QRegularExpressionMatch& match) {
  auto addr = safeCast<Address>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, addr < 256 ? OperandsFormat::ZeroPageY : OperandsFormat::AbsoluteY, addr);
}

void Assembler::handleIndirect(const QRegularExpressionMatch& match) {
  auto addr = safeCast<Address>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, OperandsFormat::Indirect, addr);
}

void Assembler::handleIndexedIndirectX(const QRegularExpressionMatch& match) {
  auto addr = safeCast<uint8_t>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, OperandsFormat::IndexedIndirectX, addr);
}

void Assembler::handleIndirectIndexedY(const QRegularExpressionMatch& match) {
  auto addr = safeCast<uint8_t>(operandValue(match.captured(FirstOperandGroup)));
  assemble(match, OperandsFormat::IndirectIndexedY, addr);
}

void Assembler::handleBranch(const QRegularExpressionMatch& match) {
  auto displacement = operandAsBranchDisplacement(match.captured(FirstOperandGroup));
  assemble(match, OperandsFormat::Branch, displacement);
}

void Assembler::assemble(const QRegularExpressionMatch& match, OperandsFormat mode, int operand) {
  const auto instruction = resolveInstruction(match.captured(OperationGroup), mode);
  addByte(static_cast<uint8_t>(std::distance(InstructionTable.begin(), instruction)));
  if (instruction->size > 1) addByte(static_cast<uint8_t>(operand));
  if (instruction->size > 2) addByte(static_cast<uint8_t>(operand >> 8));
}

AssemblyResult Assembler::processControlCommand(const AssemblerLine& line) {
  switch (line.command) {
  case ControlCommand::SetOrigin: return cmdSetOrigin(line);
  case ControlCommand::EmitBytes: return cmdEmitBytes(line);
  case ControlCommand::EmitWords: return cmdEmitWords(line);
  default: return AssemblyResult::CommandProcessingError;
  }
}

AssemblyResult Assembler::processInstruction(const AssemblerLine& line) {
  if (line.operandsFormat == NoOperands) return assemble(line.instructionType, NoOperands);
  if (line.operands.isEmpty()) return AssemblyResult::MissingOperand;

  int num;
  if (line.isOperandNumeric()) {
    num = line.operandAsNumber();
  } else if (auto opt = symbolTable.get(line.operands[0])) {
    if (line.operandsFormat == Branch) {
      num = *opt - locationCounter - line.instructionSize();
    } else {
      num = *opt;
    }
  } else {
    return AssemblyResult::SymbolNotDefined;
  }

  return assemble(line.instructionType, line.operandsFormat, num);
}

AssemblyResult Assembler::cmdSetOrigin(const AssemblerLine& line) {
  if (line.isOperandNumeric()) {
    locationCounter = static_cast<uint16_t>(line.operandAsNumber());
    return AssemblyResult::Ok;
  } else {
    return AssemblyResult::NumericOperandRequired;
  }
}

AssemblyResult Assembler::cmdEmitBytes(const AssemblerLine& line) {
  for (auto num = 0; num < line.operands.size(); num++) {
    if (!line.isOperandNumeric(num)) return AssemblyResult::NumericOperandRequired;
    addByte(static_cast<uint8_t>(line.operandAsNumber(num)));
  }
  return AssemblyResult::Ok;
}

AssemblyResult Assembler::cmdEmitWords(const AssemblerLine& line) {
  for (auto num = 0; num < line.operands.size(); num++) {
    if (!line.isOperandNumeric(num)) return AssemblyResult::NumericOperandRequired;
    const auto word = static_cast<uint16_t>(line.operandAsNumber(num));
    addByte(static_cast<uint8_t>(word));
    addByte(word >> 8);
  }
  return AssemblyResult::Ok;
}

AssemblyResult Assembler::defineSymbol(const QString& name, uint16_t value) {
  if (mode == ProcessingMode::ScanForSymbols) {
    return symbolTable.put(name, value) ? AssemblyResult::Ok : AssemblyResult::SymbolAlreadyDefined;
  }
  return AssemblyResult::Ok;
}

void Assembler::defineSymbol2(const QString& name, uint16_t value) {
  if (mode == ProcessingMode::ScanForSymbols) {
    if (!symbolTable.put(name, value)) throw AssemblyResult::SymbolAlreadyDefined;
  }
}

void Assembler::addByte(uint8_t b) {
  if (mode == ProcessingMode::EmitCode) {
    updateAddressRange(locationCounter);
    memory[locationCounter] = b;
    written++;
  }
  locationCounter++;
}

void Assembler::updateAddressRange(uint16_t addr) {
  addressRange.expand(addr);
}
